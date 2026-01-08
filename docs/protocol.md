# DLL ↔ Orchestrator Protocol

Communication protocol between the Civ V DLL and the Python orchestrator.

**Transport**: Named pipe `\\.\pipe\civv_llm`
**Format**: Newline-delimited JSON
**Encoding**: UTF-8
**Version**: 1.0

> **Command Reference**: See [`api.yaml`](api.yaml) for the complete specification of all commands, events, hooks, and error codes.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              Civilization V                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                         CvGameCoreDLL                                │    │
│  │                                                                      │    │
│  │   ┌──────────────┐      ┌──────────────┐      ┌──────────────┐      │    │
│  │   │  Game Logic  │ ───► │    Hooks     │ ───► │              │      │    │
│  │   │  (CvGame)    │      │ (Notify/Pop) │      │  GameState   │      │    │
│  │   └──────────────┘      └──────────────┘      │    Pipe      │      │    │
│  │          ▲                                    │              │──────┼────┼───► Named Pipe
│  │          │              ┌──────────────┐      │  (Threaded)  │      │    │    \\.\pipe\civv_llm
│  │          └───────────── │   Command    │ ◄─── │              │      │    │
│  │                         │   Handlers   │      └──────────────┘      │    │
│  │                         └──────────────┘                            │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────────────┘

                                     │
                                     │ Named Pipe (JSON)
                                     ▼

┌─────────────────────────────────────────────────────────────────────────────┐
│                            Python Orchestrator                               │
│                                                                              │
│   ┌──────────────┐      ┌──────────────┐      ┌──────────────┐              │
│   │  Pipe Client │ ◄──► │  MCP Server  │ ◄──► │  HTTP/SSE    │ ◄───► LLM   │
│   │              │      │              │      │   Server     │              │
│   └──────────────┘      └──────────────┘      └──────────────┘              │
│                                │                                             │
│                                ▼                                             │
│                         ┌──────────────┐                                     │
│                         │  Dashboard   │ ◄───► Browser                       │
│                         │   (Web UI)   │                                     │
│                         └──────────────┘                                     │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Threading Model

```
┌─────────────────────────────────────────────────────────────────┐
│                          DLL Process                             │
│                                                                  │
│   ┌─────────────────────┐         ┌─────────────────────┐       │
│   │    Main Thread      │         │    Pipe Thread      │       │
│   │    (Game Loop)      │         │   (Background I/O)  │       │
│   │                     │         │                     │       │
│   │  ┌───────────────┐  │         │  ┌───────────────┐  │       │
│   │  │ ProcessCmds() │◄─┼── WM ───┼──│  ReadPipe()   │  │       │
│   │  └───────────────┘  │  MSG    │  └───────────────┘  │       │
│   │         │           │         │         ▲           │       │
│   │         ▼           │         │         │           │       │
│   │  ┌───────────────┐  │         │         │           │       │
│   │  │ SendMessage() │──┼─────────┼─────────┘           │       │
│   │  └───────────────┘  │  Write  │                     │       │
│   └─────────────────────┘         └─────────────────────┘       │
└─────────────────────────────────────────────────────────────────┘

Key:
  • Pipe thread reads continuously, queues commands
  • Window message (WM_PIPE_COMMAND_READY) wakes main thread
  • Main thread processes commands during game update
  • Responses sent directly on main thread (write-locked)
```

---

## Message Flow

### Turn Lifecycle

```
     DLL                                          Orchestrator
      │                                                │
      │  ════════════ TURN START ════════════════════  │
      │                                                │
      │  {"type": "turn_start", "turn": 7, ...}        │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  ══════════════ QUERIES ══════════════════════ │
      │                                                │
      │              {"type": "get_units", ...}        │
      │ ◄───────────────────────────────────────────── │
      │  {"type": "units_result", ...}                 │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │              {"type": "get_cities", ...}       │
      │ ◄───────────────────────────────────────────── │
      │  {"type": "cities_result", ...}                │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  ═══════════════ ACTIONS ═════════════════════ │
      │                                                │
      │              {"type": "move_unit", ...}        │
      │ ◄───────────────────────────────────────────── │
      │  {"type": "move_unit_result", ...}             │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  ══════════════ TURN END ═════════════════════ │
      │                                                │
      │              {"type": "end_turn"}              │
      │ ◄───────────────────────────────────────────── │
      │  {"type": "turn_end_ack", ...}                 │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │         (AI turns process...)                  │
      │                                                │
      │  {"type": "turn_start", "turn": 8, ...}        │
      │ ─────────────────────────────────────────────► │
      │                                                │
```

### Hooks (Async Events)

```
     DLL                                          Orchestrator
      │                                                │
      │  Game event occurs (notification, popup, etc.) │
      │                                                │
      │  {"type": "notification", ...}                 │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  {"type": "popup", ...}                        │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  {"type": "diplomatic_message", ...}           │
      │ ─────────────────────────────────────────────► │
      │                                                │
      │  (No response expected - fire and forget)      │
      │                                                │
```

---

## Session Tracking

All events and hooks include two identifiers for session management:

| Field | Type | Description |
|-------|------|-------------|
| `game_id` | uint | Map random seed - unique per game, **persists across saves/loads** |
| `session_id` | uint | Generated on pipe connect - unique per connection session |

**Use cases:**
- **game_id**: Filter logs to show only current game, detect game changes
- **session_id**: Track connection sessions, detect reconnects

**Example:**
```json
{
  "type": "turn_start",
  "game_id": 1847293654,
  "session_id": 3847561,
  "turn": 7,
  ...
}
```

---

## Message Categories

| Category | Direction | Description |
|----------|-----------|-------------|
| **Commands** | Client → DLL | Request/response queries and actions |
| **Events** | DLL → Client | Unsolicited state changes (turn_start, turn_complete) |
| **Hooks** | DLL → Client | Game event notifications (popups, diplomacy, tech) |
| **Errors** | DLL → Client | Error responses to failed commands |

---

## Request/Response Pattern

All commands follow this pattern:

```json
// Request
{
  "type": "<command_name>",
  "request_id": "<correlation_id>",
  // ... command-specific fields
}

// Success Response
{
  "type": "<command_name>_result",
  "request_id": "<correlation_id>",
  // ... response fields
}

// Error Response
{
  "type": "error",
  "code": "<ERROR_CODE>",
  "message": "Human-readable description",
  "request_id": "<correlation_id>"
}
```

---

## Command Summary

| Command | Description |
|---------|-------------|
| `ping` | Connection health check |
| `get_state` | Basic game state |
| `get_player_status` | Detailed player resources/stats |
| `get_demographics` | All civs' rankings |
| `get_notifications` | Player notifications |
| `get_units` | All units for a player |
| `get_cities` | All cities for a player |
| `move_unit` | Move unit to coordinates |
| `select_unit` | Select unit in UI |
| `do_control` | Execute game control action |
| `can_do_control` | Check if control is available |
| `end_turn` | End current player's turn |

See [`api.yaml`](api.yaml) for complete request/response schemas.

---

## Hook Summary

| Hook | Trigger | Description |
|------|---------|-------------|
| `notification` | `CvNotifications::Add` | Player notifications |
| `popup` | `CvGame::AddPopupWithPipe` | UI popups |
| `diplomatic_message` | Diplomacy UI | AI diplomatic interactions |
| `tech_researched` | Tech completion | Technology researched |

---

## Implementation Notes

1. **Blocking**: Commands block until response; don't pipeline requests
2. **Times 100**: Yield values are ×100 to avoid floats (e.g., `gold_times100`)
3. **Player defaults**: Commands with `player_id` default to active player
4. **Thread safety**: DLL uses CRITICAL_SECTION for write lock
5. **Reconnection**: Pipe thread auto-reconnects on disconnect
