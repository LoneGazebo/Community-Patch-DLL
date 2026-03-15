#!/usr/bin/env python3
"""
Generate .modinfo file from a .civ5proj file.

Usage:
    python generate_modinfo.py <path_or_subpath_to_civ5proj_or_directory>
    python generate_modinfo.py "(1) Community Patch/Community Patch.civ5proj"
    python generate_modinfo.py "(1) Community Patch"
    python generate_modinfo.py "(1)"
    python generate_modinfo.py "(2) Vox Populi" --output-dir ./output

The argument can be:
  - A path to a .civ5proj file
  - A directory containing a single .civ5proj file
  - A prefix that uniquely matches a directory name in the repository
"""

import argparse
import hashlib
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Optional

NS = {'ms': 'http://schemas.microsoft.com/developer/msbuild/2003'}


def compute_md5(file_path: Path) -> str:
    hash_md5 = hashlib.md5()
    with open(file_path, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b''):
            hash_md5.update(chunk)
    return hash_md5.hexdigest().upper()


def escape_xml(text: str) -> str:
    if not text:
        return ''
    text = text.replace('&', '&amp;')
    text = text.replace('<', '&lt;')
    text = text.replace('>', '&gt;')
    text = text.replace('"', '&quot;')
    return text


def bool_to_int(value: str) -> str:
    if value is None:
        return '0'
    return '1' if value.lower() == 'true' else '0'


def get_text(element: Optional[ET.Element], default: str = '') -> str:
    if element is None:
        return default
    return element.text or default


def parse_civ5proj(civ5proj_path: Path) -> dict:
    tree = ET.parse(civ5proj_path)
    root = tree.getroot()

    prop_group = root.find('ms:PropertyGroup', NS)
    if prop_group is None:
        raise RuntimeError(f"No PropertyGroup found in {civ5proj_path}")

    data = {
        'guid': get_text(prop_group.find('ms:Guid', NS)),
        'mod_version': get_text(prop_group.find('ms:ModVersion', NS), '1'),
        'name': get_text(prop_group.find('ms:Name', NS)),
        'teaser': get_text(prop_group.find('ms:Teaser', NS)),
        'description': get_text(prop_group.find('ms:Description', NS)),
        'authors': get_text(prop_group.find('ms:Authors', NS)),
        'special_thanks': get_text(prop_group.find('ms:SpecialThanks', NS)).strip(),
        'hide_setup_game': bool_to_int(get_text(prop_group.find('ms:HideSetupGame', NS), 'false')),
        'homepage': get_text(prop_group.find('ms:Homepage', NS)).strip(),
        'affects_saved_games': bool_to_int(get_text(prop_group.find('ms:AffectsSavedGames', NS), 'true')),
        'min_compatible_save_version': get_text(prop_group.find('ms:MinCompatibleSaveVersion', NS), '0'),
        'supports_single_player': bool_to_int(get_text(prop_group.find('ms:SupportsSinglePlayer', NS), 'true')),
        'supports_multiplayer': bool_to_int(get_text(prop_group.find('ms:SupportsMultiplayer', NS), 'true')),
        'supports_hot_seat': bool_to_int(get_text(prop_group.find('ms:SupportsHotSeat', NS), 'false')),
        'supports_mac': bool_to_int(get_text(prop_group.find('ms:SupportsMac', NS), 'true')),
        'reload_audio_system': bool_to_int(get_text(prop_group.find('ms:ReloadAudioSystem', NS), 'false')),
        'reload_landmark_system': bool_to_int(get_text(prop_group.find('ms:ReloadLandmarkSystem', NS), 'false')),
        'reload_strategic_view_system': bool_to_int(get_text(prop_group.find('ms:ReloadStrategicViewSystem', NS), 'false')),
        'reload_unit_system': bool_to_int(get_text(prop_group.find('ms:ReloadUnitSystem', NS), 'false')),
        'dependencies': [],
        'references': [],
        'blocks': [],
        'actions': {},
        'entry_points': [],
        'files': [],
    }

    mod_deps = prop_group.find('ms:ModDependencies', NS)
    if mod_deps is not None:
        for assoc in mod_deps.findall('ms:Association', NS):
            data['dependencies'].append({
                'type': get_text(assoc.find('ms:Type', NS)),
                'name': get_text(assoc.find('ms:Name', NS)),
                'id': get_text(assoc.find('ms:Id', NS)),
                'min_version': get_text(assoc.find('ms:MinVersion', NS), '0'),
                'max_version': get_text(assoc.find('ms:MaxVersion', NS), '999'),
            })

    mod_refs = prop_group.find('ms:ModReferences', NS)
    if mod_refs is not None:
        for assoc in mod_refs.findall('ms:Association', NS):
            data['references'].append({
                'name': get_text(assoc.find('ms:Name', NS)),
                'id': get_text(assoc.find('ms:Id', NS)),
                'min_version': get_text(assoc.find('ms:MinVersion', NS), '0'),
                'max_version': get_text(assoc.find('ms:MaxVersion', NS), '999'),
            })

    mod_blockers = prop_group.find('ms:ModBlockers', NS)
    if mod_blockers is not None:
        for assoc in mod_blockers.findall('ms:Association', NS):
            data['blocks'].append({
                'type': get_text(assoc.find('ms:Type', NS)),
                'name': get_text(assoc.find('ms:Name', NS)),
                'id': get_text(assoc.find('ms:Id', NS)),
                'min_version': get_text(assoc.find('ms:MinVersion', NS), '0'),
                'max_version': get_text(assoc.find('ms:MaxVersion', NS), '999'),
            })

    mod_actions = prop_group.find('ms:ModActions', NS)
    if mod_actions is not None:
        for action in mod_actions.findall('ms:Action', NS):
            action_set = get_text(action.find('ms:Set', NS))
            action_type = get_text(action.find('ms:Type', NS))
            action_file = get_text(action.find('ms:FileName', NS))
            if action_set not in data['actions']:
                data['actions'][action_set] = []
            data['actions'][action_set].append({'type': action_type, 'file': action_file})

    mod_content = prop_group.find('ms:ModContent', NS)
    if mod_content is not None:
        contents = mod_content.findall('Content')
        contents.extend(mod_content.findall('ms:Content', NS))
        for content in contents:
            entry_type = get_text(content.find('ms:Type', NS))
            entry_name = get_text(content.find('ms:Name', NS))
            entry_desc = get_text(content.find('ms:Description', NS))
            entry_file = get_text(content.find('ms:FileName', NS))
            if entry_type and entry_file:
                data['entry_points'].append({
                    'type': entry_type,
                    'name': entry_name,
                    'description': entry_desc,
                    'file': entry_file,
                })

    for item_group in root.findall('ms:ItemGroup', NS):
        for content in item_group.findall('ms:Content', NS):
            include_path = content.get('Include')
            if include_path:
                import_vfs = content.find('ms:ImportIntoVFS', NS)
                import_val = '1' if import_vfs is not None and import_vfs.text == 'True' else '0'
                data['files'].append({'path': include_path, 'import': import_val})

    return data


def generate_modinfo_xml(data: dict, mod_dir: Path) -> str:
    lines = []
    lines.append('<?xml version="1.0" encoding="utf-8"?>')
    lines.append(f'<Mod id="{data["guid"]}" version="{data["mod_version"]}">')

    lines.append('  <Properties>')
    lines.append(f'    <Name>{escape_xml(data["name"])}</Name>')
    lines.append(f'    <Teaser>{escape_xml(data["teaser"])}</Teaser>')
    lines.append(f'    <Description>{escape_xml(data["description"])}</Description>')
    lines.append(f'    <Authors>{escape_xml(data["authors"])}</Authors>')
    if data['special_thanks']:
        lines.append(f'    <SpecialThanks>{escape_xml(data["special_thanks"])}</SpecialThanks>')
    lines.append(f'    <HideSetupGame>{data["hide_setup_game"]}</HideSetupGame>')
    if data['homepage']:
        lines.append(f'    <Homepage>{escape_xml(data["homepage"])}</Homepage>')
    lines.append(f'    <AffectsSavedGames>{data["affects_saved_games"]}</AffectsSavedGames>')
    if data['min_compatible_save_version'] != '0':
        lines.append(f'    <MinCompatibleSaveVersion>{data["min_compatible_save_version"]}</MinCompatibleSaveVersion>')
    lines.append(f'    <SupportsSinglePlayer>{data["supports_single_player"]}</SupportsSinglePlayer>')
    lines.append(f'    <SupportsMultiplayer>{data["supports_multiplayer"]}</SupportsMultiplayer>')
    lines.append(f'    <SupportsHotSeat>{data["supports_hot_seat"]}</SupportsHotSeat>')
    lines.append(f'    <SupportsMac>{data["supports_mac"]}</SupportsMac>')
    lines.append(f'    <ReloadAudioSystem>{data["reload_audio_system"]}</ReloadAudioSystem>')
    lines.append(f'    <ReloadLandmarkSystem>{data["reload_landmark_system"]}</ReloadLandmarkSystem>')
    lines.append(f'    <ReloadStrategicViewSystem>{data["reload_strategic_view_system"]}</ReloadStrategicViewSystem>')
    lines.append(f'    <ReloadUnitSystem>{data["reload_unit_system"]}</ReloadUnitSystem>')
    lines.append('  </Properties>')

    if data['dependencies']:
        lines.append('  <Dependencies>')
        for dep in data['dependencies']:
            if dep['type'] == 'Game':
                lines.append(f'    <Game minversion="{dep["min_version"]}" maxversion="{dep["max_version"]}" />')
            elif dep['type'] == 'Dlc':
                lines.append(f'    <Dlc id="{dep["id"]}" minversion="{dep["min_version"]}" maxversion="{dep["max_version"]}" />')
            elif dep['type'] == 'Mod':
                lines.append(f'    <Mod id="{dep["id"]}" minversion="{dep["min_version"]}" maxversion="{dep["max_version"]}" title="{escape_xml(dep["name"])}" />')
        lines.append('  </Dependencies>')
    else:
        lines.append('  <Dependencies />')

    if data['references']:
        lines.append('  <References>')
        for ref in data['references']:
            lines.append(f'    <Mod id="{ref["id"]}" minversion="{ref["min_version"]}" maxversion="{ref["max_version"]}" title="{escape_xml(ref["name"])}" />')
        lines.append('  </References>')
    else:
        lines.append('  <References />')

    if data['blocks']:
        lines.append('  <Blocks>')
        for block in data['blocks']:
            block_type = block.get('type', 'Mod')
            if block_type == 'Dlc':
                lines.append(f'    <Dlc id="{block["id"]}" minversion="{block["min_version"]}" maxversion="{block["max_version"]}" />')
            else:
                lines.append(f'    <Mod id="{block["id"]}" minversion="{block["min_version"]}" maxversion="{block["max_version"]}" title="{escape_xml(block["name"])}" />')
        lines.append('  </Blocks>')
    else:
        lines.append('  <Blocks />')

    lines.append('  <Files>')
    missing = []
    for file_info in data['files']:
        file_path = Path(file_info['path'])
        full_path = mod_dir / file_path
        if full_path.exists():
            md5_hash = compute_md5(full_path)
            normalized_path = str(file_path).replace('\\', '/')
            lines.append(f'    <File md5="{md5_hash}" import="{file_info["import"]}">{normalized_path}</File>')
        else:
            missing.append(str(full_path))
    lines.append('  </Files>')

    if data['actions']:
        lines.append('  <Actions>')
        for action_set, actions in data['actions'].items():
            lines.append(f'    <{action_set}>')
            for action in actions:
                lines.append(f'      <{action["type"]}>{action["file"]}</{action["type"]}>')
            lines.append(f'    </{action_set}>')
        lines.append('  </Actions>')

    if data['entry_points']:
        lines.append('  <EntryPoints>')
        for entry in data['entry_points']:
            entry_file = entry['file'].replace('\\', '/')
            lines.append(f'    <EntryPoint type="{entry["type"]}" file="{entry_file}">')
            lines.append(f'      <Name>{escape_xml(entry["name"])}</Name>')
            lines.append(f'      <Description>{escape_xml(entry["description"])}</Description>')
            lines.append(f'    </EntryPoint>')
        lines.append('  </EntryPoints>')

    lines.append('</Mod>')
    lines.append('')

    return '\n'.join(lines), missing


def main():
    parser = argparse.ArgumentParser(description="Generate a .modinfo file from a .civ5proj file.")
    parser.add_argument('civ5proj', type=Path, help="Path to the .civ5proj file")
    parser.add_argument(
        '--output-dir', type=Path, default=None,
        help="Directory to write the .modinfo file (default: same directory as .civ5proj)"
    )
    args = parser.parse_args()

    civ5proj_path = args.civ5proj

    # If path doesn't exist, try to find a unique match by prefix in the current directory
    if not civ5proj_path.exists():
        prefix = str(civ5proj_path)
        candidates = [p for p in Path('.').iterdir() if p.name.startswith(prefix)]
        if len(candidates) == 1:
            civ5proj_path = candidates[0]
        elif len(candidates) == 0:
            print(f"Error: File not found and no directory matches prefix '{prefix}'")
            sys.exit(1)
        else:
            print(f"Error: Ambiguous prefix '{prefix}' matches multiple entries:")
            for c in candidates:
                print(f"  {c}")
            sys.exit(1)

    # If a directory was given, find the unique .civ5proj file inside it
    if civ5proj_path.is_dir():
        proj_files = list(civ5proj_path.glob('*.civ5proj'))
        if len(proj_files) == 1:
            civ5proj_path = proj_files[0]
        elif len(proj_files) == 0:
            print(f"Error: No .civ5proj file found in '{civ5proj_path}'")
            sys.exit(1)
        else:
            print(f"Error: Multiple .civ5proj files found in '{civ5proj_path}':")
            for f in proj_files:
                print(f"  {f}")
            sys.exit(1)

    civ5proj_path = civ5proj_path.resolve()
    if not civ5proj_path.exists():
        print(f"Error: File not found: {civ5proj_path}")
        sys.exit(1)

    mod_dir = civ5proj_path.parent
    output_dir = args.output_dir.resolve() if args.output_dir else mod_dir

    print(f"Parsing: {civ5proj_path}")
    data = parse_civ5proj(civ5proj_path)

    modinfo_name = f"{data['name']} (v {data['mod_version']}).modinfo"
    modinfo_path = output_dir / modinfo_name

    print(f"Generating: {modinfo_name}")
    xml_content, missing_files = generate_modinfo_xml(data, mod_dir)

    for f in missing_files:
        print(f"  WARNING: File not found: {f}")

    output_dir.mkdir(parents=True, exist_ok=True)
    with open(modinfo_path, 'w', encoding='utf-8-sig', newline='\n') as f:
        f.write(xml_content)

    print(f"Written: {modinfo_path}")


if __name__ == '__main__':
    main()
