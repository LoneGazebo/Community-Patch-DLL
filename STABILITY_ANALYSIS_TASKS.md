# Stability analysis — what was attempted, and what to try next

A pure **code-analysis** campaign against late-game instability in the GameCoreDLL: invalid
reads/writes, integer overflow, leaks, and deliberate crashes. The reported problems were **not
reproducible**, and no runtime tooling was available, so everything here was found by reading code.

This file exists so a future effort does not repeat it. It records what ground was covered, which
techniques paid and which did not, the leads that were deliberately left alone, and the two things
worth doing next — **both of which require running the game, which is where reading ran out.**

Individual failure scenarios are not repeated here; each of the 30 code commits on this branch
states its own in full.

## What it found

| Class | Count | Representative |
|---|---|---|
| Crash on legitimate input | 5 | `City:HasCounterSpy()` on a city-state; `Player:DisbandUnit()` with a caravan owned |
| NULL deref, no mod needed | 1 | `gAssignmentStorage.peekNext()` unchecked in the tactical simulator — **the best finding** |
| Use-after-free | 1 | `CvCity::PostKill` reads a member out of the object `deleteCity` just freed |
| Leak, per game | 1 | Three `CvMap` slabs never freed — ~1.9 MB per new game or load |
| Memory-unsafe writes reachable from Lua / network | ~15 | `Game.SetPlotExtraYield`, `CvTeam::PushIgnoreWarning`, … |
| Save-correctness | 5 | 3 unserialized members, a mis-sized `Traits<>` count, an unhashed span |

~145 fixes in total; most are one-line guards on the Lua and network boundaries.

---

## Ground rules

Binding for any follow-up work. Rules 3 and 5 are what kept the false-positive rate survivable.

1. **One finding → one targeted fix.** Minimum lines that correct the defect.
2. **No refactoring.** No renames, reformatting, extracted helpers, or adjacent cleanup.
3. **No speculative hardening.** No guard without a concrete story for how the bad value arises.
4. **Match surrounding style.** C++03, MSVC v90. No `auto`, range-for, `nullptr`, smart pointers.
5. **Write the failure scenario before the fix.** No concrete inputs → it is a suspicion, not a
   finding. Record it and move on.
6. **Findings are hypotheses.** Prefer fixes that are correct either way.
7. **Build before committing, if you have the tooling.** None was available for this work — see the
   note at the end of this file — so everything here was type-checked by reading. If you can build,
   do: CI requires both MSVC and clang to be warning-free, and `-Wreorder` fires easily when
   touching a constructor initialiser list.

---

## What the codebase is

~616k LOC C++, 174 `.cpp` / 293 `.h`, C++03 / MSVC v90 / Win32. Largest files: `CvDiplomacyAI.cpp`
(58k), `CvPlayer.cpp` (51k), `CvCity.cpp` (36k), `CvUnit.cpp` (34k). Zero smart pointers, ~610 raw
`new` against ~234 `delete`. Only 141 uses of 64-bit integers in the whole tree — all game math is
32-bit `int`, including `Times100` accumulators.

**Facts that change how you read this code. These are the reusable part of the campaign.**

- **`ASSERT` compiles out in shipped builds; `PRECONDITION` and `UNREACHABLE()` do not.** The latter
  are `BUILTIN_TRAP()` — ~4,984 deliberate crash sites. So index accessors are far better hardened
  than they look, *and* every one of those sites is a crash indistinguishable from corruption if it
  can fire on legitimate input. Never treat an `ASSERT` as a guard.
- **`GC.get<Foo>Info(-1)` traps; it does not return NULL.** The most load-bearing fact here. 133 of
  150 info accessors open with `PRECONDITION(index > -1)`. The DLL does not believe this: ~1,371
  sites are written `pkInfo = GC.getFooInfo(e); if (pkInfo) {…}`. A null test on an info accessor's
  result is therefore evidence the author *expected* a graceful path — a candidate, not a guard.
- **`getInfoTypeForString` resolves against one map shared by every info table.** A string belonging
  to a different table returns *that* table's row id, so the failure mode is a plausible in-range
  integer indexing the wrong table. Validate against `[0, getNum<Thing>Infos())`, never against the
  sentinel. Matters most for `CLOSED_ENUM` members, where a foreign row id is UB under clang.
- **Both silent-container types stop checking in shipped builds and take an unsigned index.**
  `Firaxis::Array` (C runtime `assert`) and `CvEnumMap` (`ASSERT`). On Win32 a -1 index wraps to
  `base - sizeof(T)` — the field before the array, or an allocator header. No trap, no log.
- **The "linked lists" are vectors.** `CvPlot::m_units`, `CvCity::m_orderQueue`,
  `CvUnit::m_missionQueue` are `FFastSmallFixedList`; node pointers are raw pointers into a
  reallocating buffer, and `next()` is bounds-checked pointer arithmetic. Append can free the
  buffer; a dangling node fails *silently* (usually NULL, occasionally an index into a different
  element, and `deleteNode` then removes the wrong entry). Safe idiom is at `CvUnit.cpp:21066` —
  re-derive the node after anything that can touch the container.
- **Units have deferred death; cities do not.** `CvUnit::kill(true)` sets a flag and returns;
  `CvCity::kill()` runs straight into `delete`. Cached `CvUnit*` is close to a solved problem;
  cached `CvCity*` is not.
- **The safe-iteration idiom is: snapshot IDs into a local, re-look-up inside the loop, skip NULLs.**
  Deviations are the finding. `TContainer` iteration (`firstCity`/`nextCity`) is `++index` into a
  bounds-checked `GetAt`, so a mid-loop deletion silently skips an element — a correctness bug, not
  a crash.
- **`reset()` runs on load; `init()` does not**, and `reset()` is where sub-objects are built. The
  load-time contract is always "serialize the field", never "rebuild it on load". This makes the
  `SYNC_ARCHIVE_VAR`-vs-`Serialize` diff a *complete* test for missing serialization.
- **Dynamic containers are serialized as bare spans — no length, no type mapping.** For the 69
  dynamic enums the element count is a database row count, so **the save format encodes the mod set
  implicitly**. This is detected, not prevented: `calcGameDataHash` hashes 74 tables, `CvGame::Read`
  compares, and on mismatch logs *"corruption or crash is likely"* **and loads anyway**.
- **`CLOSED_ENUM` is a real marker**, and `default: UNREACHABLE()` over one is genuinely unreachable
  — except for -1, and except when the enum *grows*. `UNITAI_SKIRMISHER` / `UNITAI_SUBMARINE` were
  appended for `MOD_AI_UNIT_PRODUCTION` and every pre-existing exhaustive switch silently became
  non-exhaustive. Diffing the case list against the enum members found the only live trap in 58.
- Global caches keyed by raw object pointers: `CvCityManager::ms_kCityMap`,
  `FSerialization::citiesToCheck`, `FSerialization::unitsToCheck`.

---

## How to search this codebase

**What worked**

- **Build a set of dangerous *function names*, then filter call sites by the *shape of the
  argument*.** The single most productive technique — repeatedly turned unaffordable tasks into a
  page of hits (512 sites → 22; 838 loops → 13; 4,426 methods → 2). The filter must encode *why* a
  value is dangerous, not just that it exists.
- **Guard the boundary, not the call graph.** Three scans over internal call graphs produced one
  finding between them; the same technique at the **Lua boundary** produced 38 in one session. Start
  every trap-hunt at an entry point — `Lua/`, net-message handlers, UI callbacks — and work inward.
- **Hunt crashes directly, not undefined behaviour.** Eight sessions of reading for UB produced no
  confirmed crash cause. Reading *traps* plus *untrusted input* produced five.
- **Look for a surface the previous scan structurally could not see, not a wider net over the one it
  could.** Every finding in the campaign's most productive stretch came from this; both barren
  sessions came from re-sweeping known ground. The pattern is always an indirection that hides call
  sites from grep — registration macros (`BasicLuaMethod` / `LUAAPIIMPL` unpack by template
  deduction, ~942 invisible sites), typedefs renaming a container, traits classes, function-local
  `static`s. Ask what dispatch mechanism would hide a call site *before* budgeting a wider net.
- **Look for a good value reaching an *exhausted* container, not just a bad value reaching a good
  one.** Fixed-capacity pools that hand back the failure value their own API documents. This
  reachability argument scales with game size rather than mod content — i.e. exactly the population
  that reports crashes. The campaign's best finding came from this in twenty minutes.
- **Weight *subsystem* coverage, not just idiom coverage.** `CvTacticalAI.cpp` is 12.9k lines, runs
  per AI army per turn, holds three preallocated pools and eleven mutable file-scope globals, and
  had zero findings for most of the campaign — not because it was clean but because no scan shape
  reached it. Once a scan shape did, the last three areas probed all landed in that one file.
- **A guard that cannot do what its author thought it did** is the strongest single tell: a NULL
  check on something that can never be NULL, a `PRECONDITION` the callee cannot violate, a comment
  claiming a check the code does not make.
- **Rank containers by who scans them, not by how fast they grow**, and rank a staleness class by
  what the cached expression *depends on*, not by how long the cache lives. Growth and lifetime are
  almost never the bug in a process this short-lived.

**What produced false positives**

- **Sibling asymmetry generates leads; it never settles them.** "These four guard, this fifth does
  not" pointed at every confirmed finding — and at a long list of safe code, including five guards
  added and then reverted, and one accessor with perfect asymmetry and zero callers anywhere.
  **Require a second, independent piece of evidence**: a shipped `.lua` caller passing a sentinel,
  an author's comment describing the crash, a documented producer, or a raw unvalidated
  `lua_tointeger`. Asymmetry alone was safe every single time it was the only evidence.
  > The positive counterpart, and the strongest corroboration found: **two same-named functions
  > taking the same argument in the same shipped Lua expression, where one subscripts and the other
  > searches.** Look for that pair before the sibling.
- **This codebase guards a sentinel-returning accessor with a *predicate*, not a comparison** —
  `getProductionUnit()` ← `isProductionUnit()`, `GetOwnedReligion()` ← `OwnsReligion()`. A scan for
  `eX != NO_X` reports guarded sites as unguarded at a rate that swamps the real ones. Find the
  predicate sibling first. Biggest single source of false positives in the campaign.
- **A proximity classifier cannot settle a "was this checked?" question here, in either direction.**
  Budget a scan as a census and the *reading* as the verdict; when survivors keep turning out safe,
  resolve one callee rather than widening the window.
- **A scan that reports most of its population as defective is reporting a bug in the scan.** Hit
  five separate times.
- **A scan the task specifies is not necessarily the scan that works — check its population before
  budgeting on it.** One task's specified scan (header members vs. constructor init list) cannot
  work at all here, because a Civ5 header declares several classes and the diff cannot tell which
  owns a member. The scan that paid was a one-liner: **diff members assigned from `FNEW`/`new`
  against those passed to `SAFE_DELETE*`, per file** — an uninitialised pointer only bites when
  something also frees it, so find the frees first.
- **Cheap reachability test for anything in `Lua/`: grep the shipped `.lua` files.** Do it before
  fixing, not after. It killed an entire flagged cluster.

---

## Ground already covered

Do not re-run these without a reason the table does not already answer.

| Area probed | Outcome |
|---|---|
| Lifetime accumulators typed `int` | 1 fix. Magnitudes are mostly nowhere near overflow |
| Header members missing from `Serialize`/`reset` | 3 fixes. Best yield per token of the early work |
| Overflow in AI valuation math | 2 fixes. Prior campaigns had already fixed the big ones |
| Computed array indices without bounds validation | 5 defensive fixes, **none shown reachable**. Premise was stale |
| Cached `CvUnit*`/`CvCity*` used after destruction | 3 fixes incl. the use-after-free — which was **inside the deleting function**, not in any of its 13 callers |
| Monotonically growing containers | 1 fix (a quadratic). All four named candidates were false leads |
| Stale entries in raw-pointer-keyed registries | 0 real. The registries are correctly maintained |
| Container mutation during iteration | 2 fixes |
| Nullable-return dereferences | 1 fix. The specified scan describes one expression in one file |
| Allocation / ownership audit | 2 fixes: the `CvMap` slab leak and `m_pCulture` |
| Uninitialized member reads | 1 fix (5 `CvGlobals` pointers of 30) |
| Reachable `PRECONDITION`/`UNREACHABLE` traps | **81 fixes — the most productive area by far.** Five crashes on legitimate input |
| Lua API boundary — unvalidated mod arguments | 11 fixes |
| Derived state never rebuilt after load | **0 — premise invalid.** Four independent scans negative. **Do not re-open** |
| Silent `-1` from a failed database lookup | 9 fixes incl. an out-of-bounds *write* at load time. 861-site remainder swept clean |
| Node-list iteration (the families the mutation scan missed) | 1 fix. Exactly three such containers exist; all enumerated |
| Unguarded per-player accessors in sub-objects | 5 fixes, **5 guards added and reverted** |
| `CvEnumsUtil::Traits<>` count table | 2 fixes. One `Count()` returned the wrong table's row count — ~6 KB heap and 4.7 KB of save junk **per city** |
| Save-stream framing for database-sized containers | 1 fix. Product decision parked below |
| The network-message boundary | 2 fixes. **A network index is only dangerous if its container can shrink** |
| Values coming back *out* of Lua | 2 fixes. Mod-conditional by construction |
| Fixed-capacity pool exhaustion in the tactical sim | **8 fixes — the best finding.** Needs no mod, no Lua, no bad data; only a large late-game battle |
| `front()`/`back()` on a container that can be empty | **0 — clean.** 129 + 26 sites triaged |
| Process-lifetime `static`s in function bodies | 1 fix. Census is 174; only two cache something that can differ between calls |

**Dead ends — do not re-derive**

| Avenue | Result |
|---|---|
| Save/load field-order mismatch | Structurally prevented by the visitor serializer |
| `MAX_MAJOR_CIVS` as a runtime-varying array bound; the 43-civ build | Clean. `MOD_GLOBAL_MAX_MAJOR_CIVS` is a commented-out compile-time constant |
| `memset`/`memcpy` on a non-POD (48); `new[]` vs scalar `delete` (11) | Clean |
| Non-POD passed to a `%s` vararg (487 format calls) | Clean; the clang build's `-Wnon-pod-varargs` keeps it so |
| Object-**ID recycling** vs `IDInfo` | Clean by construction — IDs are monotonic and never reused |
| `(short)`/`(unsigned char)` narrowing (43) | Clean below 65,536 plots; a larger modded map would alias |
| Division by a count that can be zero (36) | Clean — all divide by a table size or guarded container |
| Self-recursive functions / stack depth | **Not settleable by name-matching.** A scan reports 1,219 false hits |
| `GC.get*Info()` unchecked-index idiom (861) | Latent, not live. **Do not re-open** |
| The `CvLuaPlot` team-index cluster; the `CvCityEspionage` cluster | Not reachable — every caller is guarded |
| Function-local `static` DB row-id caches (95) | Latent. Trigger is exe-side; 95 speculative edits would fail ground rule 3 |
| `default: UNREACHABLE()` over a database-driven enum (58) | Premise wrong — all 58 are over a `CLOSED_ENUM`. 1 live, from a *grown* enum |
| Out of scope entirely | Anything inside `Civilization5.exe`; multiplayer desync; heap corruption in the Firaxis allocator; re-entrancy *timing* from Lua/UI callbacks |

Also considered and deliberately not pursued: DLL/exe pointer-lifetime contracts (unknowable from
this side); float→int / NaN conversion (UBSan finds these far more cheaply than reading); a general
"O(n) scan in a per-event path" hunt (payoff is turn-time, not stability).

---

## What to do next

> Reading has taken this as far as reading goes. Both items below test by *running*.

**1. Get a minidump.** `docs/minidumps.md` documents automatic dump generation. **One minidump from
a late-game crash converts a 616k-line blind search into a targeted read of a handful of
functions.** Precedent: `b03649b4b` fixed a real `m_CurrentGreatWorks[-1]` OOB read, citing UBSan
plus a wallclock-correlated trace naming the turn, the player and the local values. Nothing
comparable came out of eight sessions of reading.

**2. Re-enable the built-in use-after-free detector.** `CvGameCoreDLLUtil/include/CvAssert.h:55`
holds an `ObjectValidator` template — a destroyed-object registry with `isObjectValid()` — sitting
behind `#if 0 // disabling Object Validation`. It was written for exactly the dangling
`CvCity*`/`CvUnit*` mode. Re-enable it in a build with `VPRELEASE_ERRORMSG` defined and run the
FireTuner autoplay from `DEVELOPMENT.md`. **It is a build-config change, not a code change.**

**Parked for the maintainer**

- On a game-data-hash mismatch at load, the game warns and loads anyway. Should it refuse?
  Same family: `CvDllDatabaseUtility::CacheGameDatabaseData` discards the result of
  `ValidateGameDatabase()` (`:122-124`, with a `//TODO`).
- One save-breaking fix was authorised on the grounds that the branch was already incompatible.

---

## Open leads — real, deliberately not fixed

Each needs one more piece of evidence, or is blocked. None is a stability finding on its own.

**Blocked on a producer — fix the moment one turns up**

- `CvCity::PostKill` (`CvCity.cpp:2239`) is still a non-static member function called on a freed
  `this`. Its one real member access was fixed; the next one anyone adds is a live bug. Clean fix is
  making it `static` — it already takes everything by parameter — but that is a signature change.
- `CvPlayer::killCities` (`:4661`) snapshots IDs but omits the NULL skip. Lua-exposed.
- `CvGame::CreateFreeCityPlayer` (`:14431`) derefs `acquireCity`'s result where two other callers
  check it. `acquireCity` returns NULL when `initCity` fails; no producer found.
- `getUnit(m_CurrentMoveUnits[i].GetID())` is NULL-checked at 21 of 24 sites in `CvTacticalAI.cpp`
  — `:2091`, `:3321`, `:4935` are not. A `PushMission` between populate and consume would do it.
- `CvTechClasses.cpp:741` / `CvPolicyClasses.cpp:6104` deref `GetEntry(getCivilizationBuildings(iI))`
  guarded only against `NO_BUILDING`; 8 of 18 siblings null-check, so the idiom is mixed.
- `m_vMonopolyAttackBonus[eDomain]` / `…Defense…` (`CvPlayer.cpp:38847`, `:38852`) — unguarded
  `vector<int>` subscripts on the **every-combat** path, indexed by a DB-loaded domain id.
- `CvPlayerTraits::IsWorkersMountainPass` (`CvTraitClasses.cpp:6913`, added upstream in VP 5.4.5)
  derefs `GC.getPromotionInfo(*it)` with no NULL check, over a set filled straight from XML and
  restored from saves.
- `CvDllCombatInfo::GetDamageMember` (`:127`) tests the upper bound and not `iIndex >= 0`, then
  returns an interior pointer to the exe.
- `CvDllPreGame::GetGameOption` (`:224`) / `GetMapOption` (`:246`) put the `+1` on the wrong operand,
  so `strcpy` writes one NUL past the end of a buffer the **exe** owns. Correct test is
  `nOptionNameBufferSize > nSourceBufferSize`. Fixable from this side.
- `CvTacticalAI::ShouldRebase` (`:5342`) traps on `UNITAI_CARRIER_AIR`, which is in the enum, marked
  `// UNUSED`, and unhandled. No shipped VP air unit uses it — fix if an air-unit mod reports a crash.
- `CvPlayer::GetHistoricEventTourism` (`:31306`) traps on members of its own enum that the enum's
  comment reserves for another function. Raw `lua_tointeger`; every C++ caller passes a literal.

**Mod-gated**

- Five mission-node bindings held across a `MOD_EVENTS_CUSTOM_MISSIONS` Lua hook
  (`CvUnitMission.cpp:708`, `:892`, `:1954`, `:2016`, plus the re-derive at `:1476` that `:1462`
  bypasses). Option ships `Value="0"`. Fix all five together — one line each.
- `CvPlayer::DoDistanceGift` (`:37715`) holds a plot unit node across `AddIncomingUnit`, which
  `initUnit`s onto the same plot. Both sibling loops re-head; this one does not.
- `CvPlayer::SetUnitClassReplacement` (`:36055`) `initUnit`s into the container it walks —
  unbounded only for a self-mapping row; the three shipped rows are all X→Y.
- `CvTeam.cpp:8156` (Zulu Impi) can append to `m_units` unboundedly. Mod-gated twice over.
- `CvPlayerCulture::ThemeBuilding` (`CvCultureClasses.cpp:1716`) holds an iterator across
  `.clear()` + `push_back`. Genuine UB *if reached*; the recompute is currently dead code.

**Boundary / API**

- `CvTeam::changeProjectCount` (`:5393`) does not validate `iChange`; `m_paiProjectCount` can go
  **negative and is serialized**, and `:5902` computes `-getProjectCount()`, which then *adds*
  projects.
- `CvNotifications.cpp:1001` — unbounded `strcpy` into a stack `char[512]` on the spy-stole-tech
  path. Reaching 512 bytes needs pathological names.
- Three sites test `strlen(s) <= sizeof(m_szCustomName)` where `sizeof` is 128 and `strcpy_s` needs
  129 — `strcpy_s` **aborts** rather than overflowing (`CvReligionClasses.cpp:1287`, `:1491`,
  `CvVotingClasses.cpp:2299`).
- `CvDllPlot::headUnitNode` (`:351`) hands a raw `IDInfo*` into the buffer to the exe. Not fixable
  from the DLL, not verifiable by reading.

**Shared mutable state**

- `CvPlot::getAllAdjacentAreas()` (`:5673`) returns a function-local `static std::vector<int>` by
  reference one statement after releasing the game-core lock. Safe today: the lock is held on every
  reachable path and all 7 callers copy by value. If a lockless caller appears, return by value.
- `CvDiplomacyRequests::ActivateNext`'s `static CvDeal kDeal` (`:295`) is never cleared between
  games. The `static` is correct — its address goes to the exe — but the human↔human branch never
  refreshes it. Holds ids, not pointers, so the symptom is a wrong scratch deal in the UI.

**Correctness only, no stability consequence** — `CvUnit::m_strUnitName` is lost on load;
`CvCity::getProductionTurnsLeft` returns a production quantity when the difference is 0;
`CvWeightedVector::ChooseAtRandom` divides by zero on an empty vector (all 33 call sites currently
guard); `m_iLifetimeCombatExperienceTimes100` accepts negative deltas; the two `CONTEST_FAITH` quest
sites narrow 64-bit lifetime faith back to `int`; `GetResearchAgreementCost` uses
`<= MAX_MAJOR_CIVS`; two `PRECONDITION`-shadowed dead range checks; the monopoly flag arrays are
serialized twice; `~CvCity`/`~CvUnit` each do a linear `std::find` over a global vector.

---

## Branch status

**`stability_improvements` is savegame-incompatible.** Exactly four commits cause it, and dropping
them makes the rest cherry-pickable for a `Z`-level release:

| Commit | Why it breaks saves |
|---|---|
| `2845286fd` | lifetime faith widened to 64-bit |
| `09044bf4c`, `0058a53c4`, `90acfbaab` | three previously unserialized members added to `Serialize` |
| `29af6292c` | corrects the `Traits<>` count, changing how many ints each city writes for three `CvEnumMap<SpecialistTypes, int>` members |

Every other commit on the branch is save-compatible.

The branch was rebased onto VP 5.4.5. Nothing on it was invalidated by the 16 upstream commits: no
hunk pair shares a function, and no fix here references any symbol upstream removed or
re-signatured. Two of those upstream commits independently fixed defects in this campaign's own bug
class. One introduced a new one — an unguarded `GET_PLAYER(ePlayer)` in `CvPlot::canHaveImprovement`
reachable with `NO_PLAYER` from a default argument — which is fixed in `87b14c7a3`.

The campaign's incremental bookkeeping history was dropped when the branch was prepared for a pull
request; this file arrives in a single commit and the 30 preceding commits are code only.

> **No commit on this branch is compile-verified.** No MSVC or clang toolchain was available in the
> environment used for this work; types were checked by inspection only. **CI must confirm both
> compilers before merge.**
