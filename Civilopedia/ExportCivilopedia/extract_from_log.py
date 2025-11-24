#!/usr/bin/env python3
"""
Extract Civilopedia JSON export from Civ5 Lua.log file.

Usage:
    python extract_from_log.py [log_file] [output_file]

Default log_file: Documents/My Games/Sid Meier's Civilization 5/Logs/Lua.log
Default output_file: civilopedia_export.json
"""

import sys
import os
import re
from pathlib import Path

def find_default_log():
    """Find the default Civ5 Lua.log location"""
    documents = Path.home() / "Documents"
    log_path = documents / "My Games" / "Sid Meier's Civilization 5" / "Logs" / "Lua.log"

    if log_path.exists():
        return str(log_path)

    return None

def extract_json_from_log(log_file, output_file):
    """Extract JSON data from Lua.log between BEGIN and END markers"""

    print(f"Reading log file: {log_file}")

    with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Find the export section
    begin_marker = "BEGIN EXPORT DATA"
    end_marker = "END EXPORT DATA"

    begin_pos = content.rfind(begin_marker)  # Use rfind to get the latest export
    if begin_pos == -1:
        print("ERROR: Could not find 'BEGIN EXPORT DATA' in log file")
        print("Make sure you've run the export in-game first")
        return False

    end_pos = content.find(end_marker, begin_pos)
    if end_pos == -1:
        print("ERROR: Could not find 'END EXPORT DATA' in log file")
        print("Export may have been interrupted")
        return False

    # Extract the section (only between the markers)
    export_section = content[begin_pos:end_pos]

    print("Found export data in log")

    # Find all lines between BEGIN and END markers
    lines = export_section.split('\n')

    json_parts = []
    chunk_count = 0

    for line in lines:
        # Clean the line first - remove timestamp and path prefixes
        # Timestamp format: [550803.546]
        cleaned_line = re.sub(r'^\[\d+\.\d+\]\s*', '', line)

        # Mod path format: \Users\Justus\Documents\...\ExportCivilopedia:
        cleaned_line = re.sub(r'^[\\\/]Users[\\\/].*?ExportCivilopedia:\s*', '', cleaned_line)

        # Also try Windows-style path
        cleaned_line = re.sub(r'^C:[\\\/]Users[\\\/].*?ExportCivilopedia:\s*', '', cleaned_line)

        # Check if this line contains chunk data
        chunk_match = re.search(r'\[CHUNK (\d+)/(\d+)\](.*)', cleaned_line)
        if chunk_match:
            chunk_num = int(chunk_match.group(1))
            total_chunks = int(chunk_match.group(2))
            chunk_data = chunk_match.group(3)

            if chunk_num == 1:
                print(f"Found {total_chunks} chunks total")

            # The actual JSON starts after the [CHUNK X/Y] marker
            json_parts.append(chunk_data)
            chunk_count += 1

    print(f"Extracted {chunk_count} chunks")

    if not json_parts:
        print("ERROR: No chunk data found")
        print("First 500 chars of export section:")
        print(export_section[:500])
        return False

    # Combine all chunks
    json_data = ''.join(json_parts)

    # Replace the placeholders back to actual newlines
    # These are the structural newlines in the JSON, not the escaped \n in strings
    json_data = json_data.replace('<!NEWLINE!>', '\n')
    json_data = json_data.replace('<!CR!>', '\r')

    # Final cleanup - remove any remaining artifacts
    # Remove delimiter lines
    json_data = re.sub(r'={3,}', '', json_data)

    json_data = json_data.strip()

    # Validate it looks like JSON
    if not json_data.strip().startswith('{'):
        print("WARNING: Extracted data doesn't start with '{' - may not be valid JSON")
        print(f"First 200 chars: {json_data[:200]}")
        print(f"Last 200 chars: {json_data[-200:]}")

    # Write to output file
    print(f"Writing to: {output_file}")

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(json_data)

    print(f"SUCCESS! Extracted {len(json_data)} bytes")
    print(f"Output saved to: {output_file}")

    # Try to validate JSON
    try:
        import json
        with open(output_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        print(f"JSON validation: PASSED")
        print(f"  - Civilizations: {len(data.get('civilizations', []))}")
        print(f"  - Units: {len(data.get('units', []))}")
        print(f"  - Buildings: {len(data.get('buildings', []))}")
        print(f"  - Technologies: {len(data.get('technologies', []))}")
    except Exception as e:
        print(f"WARNING: JSON validation failed: {e}")

    return True

def main():
    if len(sys.argv) > 1 and sys.argv[1] in ['-h', '--help', 'help']:
        print(__doc__)
        sys.exit(0)

    # Get log file path
    if len(sys.argv) > 1:
        log_file = sys.argv[1]
    else:
        log_file = find_default_log()
        if not log_file:
            print("ERROR: Could not find default Lua.log location")
            print("Please specify the log file path manually:")
            print("  python extract_from_log.py <path_to_Lua.log>")
            sys.exit(1)

    # Get output file path
    output_file = sys.argv[2] if len(sys.argv) > 2 else "civilopedia_export.json"

    # Check log file exists
    if not os.path.exists(log_file):
        print(f"ERROR: Log file not found: {log_file}")
        sys.exit(1)

    # Extract
    success = extract_json_from_log(log_file, output_file)

    if success:
        print("\nNext step: Generate HTML")
        print(f"  python generate_civilopedia.py")
    else:
        sys.exit(1)

if __name__ == '__main__':
    main()
