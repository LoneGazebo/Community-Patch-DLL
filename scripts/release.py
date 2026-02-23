#!/usr/bin/env python3
"""
Vox Populi Complete Release Script

This script automates the full release process (Steps 1-13):

Phase 1 - Preparation (Steps 1-8):
  1. Ensure we're on the latest master branch
  2. Create Debug folder structure
  3. Update version numbers based on savegame compatibility
  4. Update displayed version numbers in all relevant files
  5. Create release commit and annotated tag
  6. Build standard Release and Debug DLLs
  7. Build 43 Civ Release and Debug DLLs
  8. Undo commit and tag (to be recreated after modinfo generation)

Phase 2 - Mod Building (Steps 9-10):
  9. Copy mod folders to Build directory and generate .modinfo files
  10. Copy .modinfo files from mod folders to Build directory

Phase 3 - Finalization (Steps 11-12):
  11. Compile the autoinstaller using Inno Setup
  12. Create the final release commit and annotated tag

Usage:
    python release.py <new_version>
    python release.py 5.2.0
    python release.py 5.2.0 --skip-build
    python release.py 5.2.0 --debug-folder "C:\\Release\\Debug"
"""

import argparse
import hashlib
import os
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Dict, List, Optional, Tuple


# Project paths
SCRIPT_DIR = Path(__file__).parent.resolve()
PROJECT_DIR = SCRIPT_DIR.parent.resolve()

# Mod folders and their .civ5proj files
MODS = {
    "(1) Community Patch": "Community Patch.civ5proj",
    "(2) Vox Populi": "Vox Populi.civ5proj",
    "(3a) VP - EUI Compatibility Files": "VP EUI Compatibility.civ5proj",
    "(3b) 43 Civs Community Patch": "43 Civs Community Patch.civ5proj",
    "(4a) Squads for VP": "Squads.civ5proj",
}

# Files to update
CUSTOM_MODS_H = PROJECT_DIR / "CvGameCoreDLL_Expansion2" / "CustomMods.h"
CUSTOM_MODS_GLOBAL_H = PROJECT_DIR / "CvGameCoreDLLUtil" / "include" / "CustomModsGlobal.h"
VP_SETUP_ISS = PROJECT_DIR / "VPSetupData.iss"
BUG_REPORT_YML = PROJECT_DIR / ".github" / "ISSUE_TEMPLATE" / "bug_report_v5.yml"
BUILD_DIR = PROJECT_DIR / "Build"
OUTPUT_DIR = PROJECT_DIR / "Output"

# Inno Setup paths (common installation locations)
INNO_SETUP_PATHS = [
    Path(r"C:\Program Files (x86)\Inno Setup 6\ISCC.exe"),
    Path(r"C:\Program Files\Inno Setup 6\ISCC.exe"),
    Path(r"C:\Program Files (x86)\Inno Setup 5\ISCC.exe"),
    Path(r"C:\Program Files\Inno Setup 5\ISCC.exe"),
]

# MSBuild namespace for parsing .civ5proj files
NS = {'ms': 'http://schemas.microsoft.com/developer/msbuild/2003'}


# =============================================================================
# Utility Functions
# =============================================================================

def parse_version(version_str: str) -> Tuple[int, int, int]:
    """Parse version string X.Y.Z into tuple (X, Y, Z)."""
    match = re.match(r'^(\d+)\.(\d+)\.(\d+)$', version_str)
    if not match:
        raise ValueError(f"Invalid version format: {version_str}. Expected X.Y.Z")
    return int(match.group(1)), int(match.group(2)), int(match.group(3))


def is_savegame_compatible(old_version: str, new_version: str) -> bool:
    """
    Determine if the new release is savegame compatible.
    Compatible if only Z differs (X.Y.Z where X and Y are the same).
    """
    old_x, old_y, _ = parse_version(old_version)
    new_x, new_y, _ = parse_version(new_version)
    return old_x == new_x and old_y == new_y


def run_git_command(args: list, check: bool = True) -> subprocess.CompletedProcess:
    """Run a git command and return the result."""
    cmd = ['git'] + args
    result = subprocess.run(cmd, cwd=PROJECT_DIR, capture_output=True, text=True)
    if check and result.returncode != 0:
        print(f"Git command failed: {' '.join(cmd)}")
        print(f"stderr: {result.stderr}")
        raise RuntimeError(f"Git command failed: {result.stderr}")
    return result


def compute_md5(file_path: Path) -> str:
    """Compute MD5 hash of a file."""
    hash_md5 = hashlib.md5()
    with open(file_path, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b''):
            hash_md5.update(chunk)
    return hash_md5.hexdigest().upper()


def escape_xml(text: str) -> str:
    """Escape special XML characters."""
    if not text:
        return ''
    text = text.replace('&', '&amp;')
    text = text.replace('<', '&lt;')
    text = text.replace('>', '&gt;')
    text = text.replace('"', '&quot;')
    return text


def bool_to_int(value: str) -> str:
    """Convert string boolean to '0' or '1'."""
    if value is None:
        return '0'
    return '1' if value.lower() == 'true' else '0'


def get_text(element: Optional[ET.Element], default: str = '') -> str:
    """Get text content from element or return default."""
    if element is None:
        return default
    return element.text or default


# =============================================================================
# Step 1: Git Status Check
# =============================================================================

def check_git_status():
    """Ensure we're on master branch and working directory is clean."""
    # Check current branch
    result = run_git_command(['rev-parse', '--abbrev-ref', 'HEAD'])
    current_branch = result.stdout.strip()
    if current_branch != 'master':
        print(f"WARNING: You are on branch '{current_branch}', not 'master'.")
        response = input("Do you want to continue anyway? (y/N): ")
        if response.lower() != 'y':
            sys.exit(1)

    # Check for uncommitted changes
    result = run_git_command(['status', '--porcelain'])
    if result.stdout.strip():
        print("WARNING: You have uncommitted changes:")
        print(result.stdout)
        response = input("Do you want to continue anyway? (y/N): ")
        if response.lower() != 'y':
            sys.exit(1)

    # Fetch latest from remote
    print("Fetching latest from remote...")
    run_git_command(['fetch', 'origin'], check=False)

    # Check if we're behind
    result = run_git_command(['rev-list', '--count', 'HEAD..origin/master'], check=False)
    if result.returncode == 0:
        behind_count = int(result.stdout.strip() or 0)
        if behind_count > 0:
            print(f"WARNING: Your branch is {behind_count} commits behind origin/master.")
            response = input("Do you want to pull the latest changes? (Y/n): ")
            if response.lower() != 'n':
                run_git_command(['pull', 'origin', 'master'])


# =============================================================================
# Step 2: Debug Folder Creation
# =============================================================================

def create_debug_folder(debug_folder: Path) -> Tuple[Path, Path]:
    """Create Debug folder with Standard and 43 Civ subfolders."""
    print(f"Creating debug folder structure at: {debug_folder}")
    standard_folder = debug_folder / "Standard"
    civ43_folder = debug_folder / "43 Civ"

    standard_folder.mkdir(parents=True, exist_ok=True)
    civ43_folder.mkdir(parents=True, exist_ok=True)

    print(f"  Created: {standard_folder}")
    print(f"  Created: {civ43_folder}")
    return standard_folder, civ43_folder


# =============================================================================
# Steps 3-4: Version Number Updates
# =============================================================================

def get_current_version_from_iss() -> str:
    """Extract current version from VPSetupData.iss."""
    with open(VP_SETUP_ISS, 'r', encoding='utf-8') as f:
        content = f.read()
    match = re.search(r'#define MyAppVersion "([^"]+)"', content)
    if not match:
        raise RuntimeError("Could not find MyAppVersion in VPSetupData.iss")
    return match.group(1)


def get_current_mod_version() -> int:
    """Extract current ModVersion from Community Patch .civ5proj."""
    civ5proj_path = PROJECT_DIR / "(1) Community Patch" / "Community Patch.civ5proj"
    with open(civ5proj_path, 'r', encoding='utf-8') as f:
        content = f.read()
    match = re.search(r'<ModVersion>(\d+)</ModVersion>', content)
    if not match:
        raise RuntimeError("Could not find ModVersion in Community Patch.civ5proj")
    return int(match.group(1))


def update_teaser_version(file_path: Path, old_version: str, new_version: str):
    """Update the version in <Teaser> tag."""
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Replace version in Teaser tag: (X.Y.Z) -> (X.Y.Z)
    pattern = rf'\({re.escape(old_version)}\)'
    replacement = f'({new_version})'
    new_content = re.sub(pattern, replacement, content)

    if content != new_content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"  Updated teaser version in: {file_path.name}")
    else:
        print(f"  No teaser version change needed in: {file_path.name}")


def update_mod_version_numbers(old_mod_version: int, new_mod_version: int):
    """Update ModVersion and MinCompatibleSaveVersion in Community Patch."""
    civ5proj_path = PROJECT_DIR / "(1) Community Patch" / "Community Patch.civ5proj"

    with open(civ5proj_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Update ModVersion
    content = re.sub(
        r'<ModVersion>\d+</ModVersion>',
        f'<ModVersion>{new_mod_version}</ModVersion>',
        content
    )

    # Update MinCompatibleSaveVersion
    content = re.sub(
        r'<MinCompatibleSaveVersion>\d+</MinCompatibleSaveVersion>',
        f'<MinCompatibleSaveVersion>{new_mod_version}</MinCompatibleSaveVersion>',
        content
    )

    with open(civ5proj_path, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f"  Updated ModVersion and MinCompatibleSaveVersion to {new_mod_version}")


def update_dependency_min_versions(old_mod_version: int, new_mod_version: int):
    """Update MinVersion of Community Patch dependency in other mods."""
    dependent_mods = [
        "(2) Vox Populi",
        "(3a) VP - EUI Compatibility Files",
        "(3b) 43 Civs Community Patch",
        "(4a) Squads for VP",
    ]

    for mod_folder in dependent_mods:
        civ5proj_name = MODS[mod_folder]
        civ5proj_path = PROJECT_DIR / mod_folder / civ5proj_name

        if not civ5proj_path.exists():
            print(f"  WARNING: {civ5proj_path} not found, skipping")
            continue

        with open(civ5proj_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Update MinVersion for Community Patch dependency
        old_pattern = f'<MinVersion>{old_mod_version}</MinVersion>'
        new_pattern = f'<MinVersion>{new_mod_version}</MinVersion>'

        if old_pattern in content:
            content = content.replace(old_pattern, new_pattern)
            with open(civ5proj_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"  Updated MinVersion in: {civ5proj_name}")


def update_custom_mods_h(old_mod_version: int, new_mod_version: int):
    """Update version numbers in CustomMods.h."""
    with open(CUSTOM_MODS_H, 'r', encoding='utf-8') as f:
        content = f.read()

    # Update MOD_DLL_VERSION_NUMBER
    content = re.sub(
        r'#define MOD_DLL_VERSION_NUMBER \(\(uint\) \d+\)',
        f'#define MOD_DLL_VERSION_NUMBER ((uint) {new_mod_version})',
        content
    )

    # Update MOD_DLL_NAME
    content = re.sub(
        r'#define MOD_DLL_NAME "Community Patch v\d+',
        f'#define MOD_DLL_NAME "Community Patch v{new_mod_version}',
        content
    )

    with open(CUSTOM_MODS_H, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f"  Updated CustomMods.h version to {new_mod_version}")


def update_vp_setup_iss(old_version: str, new_version: str):
    """Update MyAppVersion in VPSetupData.iss."""
    with open(VP_SETUP_ISS, 'r', encoding='utf-8') as f:
        content = f.read()

    content = re.sub(
        rf'#define MyAppVersion "{re.escape(old_version)}"',
        f'#define MyAppVersion "{new_version}"',
        content
    )

    with open(VP_SETUP_ISS, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f"  Updated VPSetupData.iss version to {new_version}")


def update_bug_report_yml(old_version: str, new_version: str):
    """Update version in bug_report_v5.yml."""
    with open(BUG_REPORT_YML, 'r', encoding='utf-8') as f:
        content = f.read()

    # Add new version to dropdown options if not already in the options list
    version_in_options = re.search(rf'^\s*- {re.escape(new_version)}$', content, re.MULTILINE)
    if not version_in_options:
        # Find the options section and add the new version at the top
        pattern = r'(options:\r?\n)(        - )'
        replacement = rf'\g<1>        - {new_version}\n\g<2>'
        content = re.sub(pattern, replacement, content, count=1)

    # Update the description with current version
    content = re.sub(
        r'Current Version: [0-9]+\.[0-9]+\.[0-9]+',
        f'Current Version: {new_version}',
        content
    )

    with open(BUG_REPORT_YML, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f"  Updated bug_report_v5.yml with version {new_version}")


def update_all_teaser_versions(old_version: str, new_version: str):
    """Update teaser versions in all .civ5proj files."""
    for mod_folder, civ5proj_name in MODS.items():
        civ5proj_path = PROJECT_DIR / mod_folder / civ5proj_name
        if civ5proj_path.exists():
            update_teaser_version(civ5proj_path, old_version, new_version)
        else:
            print(f"  WARNING: {civ5proj_path} not found")


# =============================================================================
# Step 5: Create Release Commit and Tag
# =============================================================================

def create_release_commit_and_tag(version: str):
    """Create the release commit and annotated tag."""
    # Stage all changes
    run_git_command(['add', '-A'])

    # Create commit
    commit_message = f"{version} Release"
    run_git_command(['commit', '-m', commit_message])
    print(f"  Created commit: {commit_message}")

    # Create annotated tag
    tag_name = f"Release-{version}"

    # Check if tag already exists
    result = run_git_command(['tag', '-l', tag_name], check=False)
    if result.stdout.strip():
        print(f"  WARNING: Tag {tag_name} already exists.")
        response = input("  Do you want to delete it and create a new one? (y/N): ")
        if response.lower() == 'y':
            run_git_command(['tag', '-d', tag_name])
            print(f"  Deleted existing tag: {tag_name}")
        else:
            print("  Skipping tag creation")
            return

    run_git_command(['tag', '-a', tag_name, '-m', commit_message])
    print(f"  Created annotated tag: {tag_name}")

    # Verify tag
    result = run_git_command(['describe', '--tags', 'HEAD'])
    print(f"  Verified tag: {result.stdout.strip()}")


# =============================================================================
# Steps 6-7: DLL Building
# =============================================================================

def enable_43_civ_build():
    """Uncomment the 43 Civ define in CustomModsGlobal.h."""
    with open(CUSTOM_MODS_GLOBAL_H, 'r', encoding='utf-8') as f:
        content = f.read()

    # Uncomment the 43 civ line
    content = content.replace(
        '// #define MOD_GLOBAL_MAX_MAJOR_CIVS (43)',
        '#define MOD_GLOBAL_MAX_MAJOR_CIVS (43)'
    )

    with open(CUSTOM_MODS_GLOBAL_H, 'w', encoding='utf-8') as f:
        f.write(content)

    print("  Enabled 43 Civ build in CustomModsGlobal.h")


def disable_43_civ_build():
    """Re-comment the 43 Civ define in CustomModsGlobal.h."""
    with open(CUSTOM_MODS_GLOBAL_H, 'r', encoding='utf-8') as f:
        content = f.read()

    # Re-comment the 43 civ line (only if it's not already commented)
    content = re.sub(
        r'^#define MOD_GLOBAL_MAX_MAJOR_CIVS \(43\)',
        '// #define MOD_GLOBAL_MAX_MAJOR_CIVS (43)',
        content,
        flags=re.MULTILINE
    )

    with open(CUSTOM_MODS_GLOBAL_H, 'w', encoding='utf-8') as f:
        f.write(content)

    print("  Disabled 43 Civ build in CustomModsGlobal.h")


def build_dll(config: str) -> bool:
    """Build the DLL using the clang build script."""
    print(f"  Building {config} DLL...")
    build_script = PROJECT_DIR / "build_vp_clang.py"

    if not build_script.exists():
        print(f"  ERROR: Build script not found: {build_script}")
        return False

    result = subprocess.run(
        [sys.executable, str(build_script), '--config', config],
        cwd=PROJECT_DIR,
        capture_output=False
    )

    if result.returncode != 0:
        print(f"  ERROR: Build failed with return code {result.returncode}")
        return False

    print(f"  {config.capitalize()} build completed successfully")
    return True


def copy_dll_and_pdb(config: str, destination: Path):
    """Copy the built DLL and PDB to the destination folder."""
    config_dir = "Release" if config == "release" else "Debug"
    output_dir = PROJECT_DIR / "clang-output" / config_dir

    dll_path = output_dir / "CvGameCore_Expansion2.dll"
    pdb_path = output_dir / "CvGameCore_Expansion2.pdb"

    if dll_path.exists():
        shutil.copy2(dll_path, destination / "CvGameCore_Expansion2.dll")
        print(f"  Copied DLL to: {destination}")
    else:
        print(f"  WARNING: DLL not found at {dll_path}")

    if pdb_path.exists():
        shutil.copy2(pdb_path, destination / "CvGameCore_Expansion2.pdb")
        print(f"  Copied PDB to: {destination}")
    else:
        print(f"  WARNING: PDB not found at {pdb_path}")


def copy_release_dll_to_mod(is_43_civ: bool = False):
    """Copy the release DLL to the appropriate mod folder."""
    output_dir = PROJECT_DIR / "clang-output" / "Release"
    dll_path = output_dir / "CvGameCore_Expansion2.dll"

    if is_43_civ:
        destination = PROJECT_DIR / "(3b) 43 Civs Community Patch" / "CvGameCore_Expansion2.dll"
    else:
        destination = PROJECT_DIR / "(1) Community Patch" / "CvGameCore_Expansion2.dll"

    if dll_path.exists():
        shutil.copy2(dll_path, destination)
        print(f"  Copied release DLL to: {destination}")
    else:
        print(f"  WARNING: Release DLL not found at {dll_path}")


def cleanup_build_folders():
    """Delete clang-output and clang-build folders."""
    folders_to_delete = [
        PROJECT_DIR / "clang-output",
        PROJECT_DIR / "clang-build",
    ]

    for folder in folders_to_delete:
        if folder.exists():
            shutil.rmtree(folder)
            print(f"  Deleted: {folder}")


# =============================================================================
# Step 8: Undo Commit and Tag
# =============================================================================

def undo_commit_and_tag(version: str):
    """Undo the release commit and tag (Step 8)."""
    tag_name = f"Release-{version}"

    # Delete tag
    run_git_command(['tag', '-d', tag_name])
    print(f"  Deleted tag: {tag_name}")

    # Undo commit (soft reset)
    run_git_command(['reset', '--soft', 'HEAD~1'])
    print("  Undid commit (soft reset)")


# =============================================================================
# Step 9: Build Mods (Generate Modinfo Files)
# =============================================================================

def parse_civ5proj(civ5proj_path: Path) -> dict:
    """Parse a .civ5proj file and extract all relevant information."""
    tree = ET.parse(civ5proj_path)
    root = tree.getroot()

    # Find the first PropertyGroup (contains mod metadata)
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

    # Parse ModDependencies
    mod_deps = prop_group.find('ms:ModDependencies', NS)
    if mod_deps is not None:
        for assoc in mod_deps.findall('ms:Association', NS):
            dep_type = get_text(assoc.find('ms:Type', NS))
            dep_name = get_text(assoc.find('ms:Name', NS))
            dep_id = get_text(assoc.find('ms:Id', NS))
            dep_min = get_text(assoc.find('ms:MinVersion', NS), '0')
            dep_max = get_text(assoc.find('ms:MaxVersion', NS), '999')
            data['dependencies'].append({
                'type': dep_type,
                'name': dep_name,
                'id': dep_id,
                'min_version': dep_min,
                'max_version': dep_max,
            })

    # Parse ModReferences
    mod_refs = prop_group.find('ms:ModReferences', NS)
    if mod_refs is not None:
        for assoc in mod_refs.findall('ms:Association', NS):
            ref_name = get_text(assoc.find('ms:Name', NS))
            ref_id = get_text(assoc.find('ms:Id', NS))
            ref_min = get_text(assoc.find('ms:MinVersion', NS), '0')
            ref_max = get_text(assoc.find('ms:MaxVersion', NS), '999')
            data['references'].append({
                'name': ref_name,
                'id': ref_id,
                'min_version': ref_min,
                'max_version': ref_max,
            })

    # Parse ModBlockers
    mod_blockers = prop_group.find('ms:ModBlockers', NS)
    if mod_blockers is not None:
        for assoc in mod_blockers.findall('ms:Association', NS):
            block_type = get_text(assoc.find('ms:Type', NS))
            block_name = get_text(assoc.find('ms:Name', NS))
            block_id = get_text(assoc.find('ms:Id', NS))
            block_min = get_text(assoc.find('ms:MinVersion', NS), '0')
            block_max = get_text(assoc.find('ms:MaxVersion', NS), '999')
            data['blocks'].append({
                'type': block_type,
                'name': block_name,
                'id': block_id,
                'min_version': block_min,
                'max_version': block_max,
            })

    # Parse ModActions
    mod_actions = prop_group.find('ms:ModActions', NS)
    if mod_actions is not None:
        for action in mod_actions.findall('ms:Action', NS):
            action_set = get_text(action.find('ms:Set', NS))
            action_type = get_text(action.find('ms:Type', NS))
            action_file = get_text(action.find('ms:FileName', NS))

            if action_set not in data['actions']:
                data['actions'][action_set] = []
            data['actions'][action_set].append({
                'type': action_type,
                'file': action_file,
            })

    # Parse ModContent (entry points)
    mod_content = prop_group.find('ms:ModContent', NS)
    if mod_content is not None:
        # Try both with and without namespace
        contents = mod_content.findall('Content')
        contents.extend(mod_content.findall('ms:Content', NS))

        for content in contents:
            entry_type = get_text(content.find('Type')) or get_text(content.find('ms:Type', NS))
            entry_name = get_text(content.find('Name')) or get_text(content.find('ms:Name', NS))
            entry_desc = get_text(content.find('Description')) or get_text(content.find('ms:Description', NS))
            entry_file = get_text(content.find('FileName')) or get_text(content.find('ms:FileName', NS))
            if entry_type and entry_file:
                data['entry_points'].append({
                    'type': entry_type,
                    'name': entry_name,
                    'description': entry_desc,
                    'file': entry_file,
                })

    # Parse ItemGroup for Content files
    for item_group in root.findall('ms:ItemGroup', NS):
        for content in item_group.findall('ms:Content', NS):
            include_path = content.get('Include')
            if include_path:
                import_vfs = content.find('ms:ImportIntoVFS', NS)
                import_val = '1' if import_vfs is not None and import_vfs.text == 'True' else '0'
                data['files'].append({
                    'path': include_path,
                    'import': import_val,
                })

    return data


def generate_modinfo_xml(data: dict, mod_dir: Path) -> str:
    """Generate modinfo XML content from parsed civ5proj data."""
    lines = []
    lines.append('<?xml version="1.0" encoding="utf-8"?>')
    lines.append(f'<Mod id="{data["guid"]}" version="{data["mod_version"]}">')

    # Properties section
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

    # Dependencies section
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

    # References section
    if data['references']:
        lines.append('  <References>')
        for ref in data['references']:
            lines.append(f'    <Mod id="{ref["id"]}" minversion="{ref["min_version"]}" maxversion="{ref["max_version"]}" title="{escape_xml(ref["name"])}" />')
        lines.append('  </References>')
    else:
        lines.append('  <References />')

    # Blocks section
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

    # Files section
    lines.append('  <Files>')
    for file_info in data['files']:
        file_path = Path(file_info['path'])
        full_path = mod_dir / file_path
        if full_path.exists():
            md5_hash = compute_md5(full_path)
            # Convert backslashes to forward slashes
            normalized_path = str(file_path).replace('\\', '/')
            lines.append(f'    <File md5="{md5_hash}" import="{file_info["import"]}">{normalized_path}</File>')
        else:
            print(f"  WARNING: File not found: {full_path}")
    lines.append('  </Files>')

    # Actions section
    if data['actions']:
        lines.append('  <Actions>')
        for action_set, actions in data['actions'].items():
            lines.append(f'    <{action_set}>')
            for action in actions:
                action_type = action['type']
                action_file = action['file']
                lines.append(f'      <{action_type}>{action_file}</{action_type}>')
            lines.append(f'    </{action_set}>')
        lines.append('  </Actions>')

    # EntryPoints section
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

    return '\n'.join(lines)


def generate_modinfo_for_mod(mod_folder: str, civ5proj_name: str) -> Tuple[Optional[Path], Optional[dict]]:
    """Generate modinfo file for a specific mod. Returns (modinfo_path, parsed_data)."""
    mod_dir = PROJECT_DIR / mod_folder
    civ5proj_path = mod_dir / civ5proj_name

    if not civ5proj_path.exists():
        print(f"  ERROR: {civ5proj_path} not found")
        return None, None

    print(f"  Processing: {mod_folder}")

    # Parse civ5proj
    data = parse_civ5proj(civ5proj_path)

    # Generate modinfo filename: (X) Mod Name (v Y).modinfo
    modinfo_name = f"{data['name']} (v {data['mod_version']}).modinfo"
    modinfo_path = mod_dir / modinfo_name

    # Generate XML content
    xml_content = generate_modinfo_xml(data, mod_dir)

    # Write to file with UTF-8 BOM
    with open(modinfo_path, 'w', encoding='utf-8-sig', newline='\n') as f:
        f.write(xml_content)

    print(f"    Generated: {modinfo_name}")
    return modinfo_path, data


def copy_mod_to_build(mod_folder: str, file_list: List[dict]) -> Optional[Path]:
    """Copy only files listed in the .civ5proj to Build directory."""
    src_dir = PROJECT_DIR / mod_folder
    dest_dir = BUILD_DIR / mod_folder

    if not src_dir.exists():
        print(f"  WARNING: Source folder not found: {src_dir}")
        return None

    # Create destination directory
    dest_dir.mkdir(parents=True, exist_ok=True)

    for file_info in file_list:
        file_path = Path(file_info['path'])
        src_file = src_dir / file_path
        dest_file = dest_dir / file_path

        if src_file.exists():
            dest_file.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src_file, dest_file)
        else:
            print(f"  WARNING: File listed in .civ5proj not found: {src_file}")

    print(f"  Copied {len(file_list)} files: {mod_folder} -> Build/")
    return dest_dir


def build_mods():
    """Build all mods by copying to Build folder and generating modinfo files (Steps 9-10)."""
    
    print("\n[STEP 9] Building mods (generating modinfo files)...")
    print("Building mods (copying and generating modinfo files)...")

    # Create Build directory if it doesn't exist
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    generated_modinfo_files = []

    for mod_folder, civ5proj_name in MODS.items():
        # Generate modinfo in the mod folder
        modinfo_path, data = generate_modinfo_for_mod(mod_folder, civ5proj_name)

        if modinfo_path:
            generated_modinfo_files.append((mod_folder, modinfo_path))

        # Copy only files listed in .civ5proj to Build directory
        if data:
            copy_mod_to_build(mod_folder, data['files'])
  
    print(f"\n  Built {len(generated_modinfo_files)} mods successfully")
    
    # Copy generated modinfo files to Build directory as well
    print("\n[STEP 10] Copying modinfo files to Build directory...")
    for mod_folder, modinfo_path in generated_modinfo_files:
        if modinfo_path.exists():
            dest_path = BUILD_DIR / mod_folder / modinfo_path.name
            shutil.copy2(modinfo_path, dest_path)
            print(f"    Copied: {modinfo_path.name} -> Build/{mod_folder}/")



# =============================================================================
# Step 11: Compile Installer
# =============================================================================

def find_inno_setup_compiler() -> Path:
    """Find the Inno Setup Compiler executable."""
    for path in INNO_SETUP_PATHS:
        if path.exists():
            return path

    # Try to find it in PATH
    result = shutil.which("ISCC")
    if result:
        return Path(result)

    raise RuntimeError(
        "Inno Setup Compiler (ISCC.exe) not found. Please install Inno Setup or add it to PATH.\n"
        "Download from: https://jrsoftware.org/isinfo.php"
    )


def compile_installer():
    """Compile the autoinstaller using Inno Setup (Step 10)."""
    print("Compiling autoinstaller...")

    if not VP_SETUP_ISS.exists():
        raise RuntimeError(f"VPSetupData.iss not found at: {VP_SETUP_ISS}")

    iscc = find_inno_setup_compiler()
    print(f"  Using Inno Setup Compiler: {iscc}")

    result = subprocess.run(
        [str(iscc), str(VP_SETUP_ISS)],
        cwd=PROJECT_DIR,
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(f"  ISCC stdout: {result.stdout}")
        print(f"  ISCC stderr: {result.stderr}")
        raise RuntimeError("Failed to compile autoinstaller")

    # Find the compiled installer
    if OUTPUT_DIR.exists():
        installers = list(OUTPUT_DIR.glob("*.exe"))
        if installers:
            print(f"  Compiled installer: {installers[0]}")
        else:
            print("  WARNING: No .exe found in Output folder")
    else:
        print(f"  WARNING: Output folder not found at {OUTPUT_DIR}")

    print("  Autoinstaller compiled successfully")


# =============================================================================
# Step 13: Push Release
# =============================================================================

def verify_tag_exists(version: str) -> bool:
    """Verify that the release tag exists locally."""
    tag_name = f"Release-{version}"
    result = run_git_command(['tag', '-l', tag_name], check=False)
    return bool(result.stdout.strip())


def verify_commit_message(version: str) -> bool:
    """Verify that HEAD has the expected commit message."""
    result = run_git_command(['log', '-1', '--format=%s', 'HEAD'])
    expected_message = f"{version} Release"
    actual_message = result.stdout.strip()
    return actual_message == expected_message


def push_release(version: str, dry_run: bool = False):
    """Push the release commit and tag to GitHub (Step 13)."""
    tag_name = f"Release-{version}"

    # Push commit
    print("  Pushing commit to origin/master...")
    if dry_run:
        print("    [DRY RUN] Would run: git push origin master")
    else:
        run_git_command(['push', 'origin', 'master'])
        print("    Commit pushed successfully")

    # Push tag
    print(f"  Pushing tag {tag_name} to origin...")
    if dry_run:
        print(f"    [DRY RUN] Would run: git push origin {tag_name}")
    else:
        run_git_command(['push', 'origin', tag_name])
        print("    Tag pushed successfully")

    # Verify tag on remote
    if not dry_run:
        print(f"  Verifying tag on remote...")
        result = run_git_command(['ls-remote', '--tags', 'origin'], check=False)
        if result.returncode == 0 and tag_name in result.stdout:
            print(f"    Tag {tag_name} is visible on GitHub")
        else:
            print(f"    WARNING: Tag {tag_name} may not be visible on remote")


# =============================================================================
# Main Function
# =============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Complete Vox Populi release automation (Steps 1-13)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        'new_version',
        help="New version number in X.Y.Z format (e.g., 5.2.0)"
    )
    parser.add_argument(
        '--debug-folder',
        type=Path,
        help="Path to the Debug folder for storing debug DLLs"
    )
    parser.add_argument(
        '--skip-build',
        action='store_true',
        help="Skip building the DLLs (only update version numbers)"
    )
    parser.add_argument(
        '--skip-git-check',
        action='store_true',
        help="Skip git branch and status checks"
    )
    parser.add_argument(
        '--skip-compile',
        action='store_true',
        help="Skip compiling the autoinstaller (Step 10)"
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help="Show what would be done for push without actually pushing"
    )

    args = parser.parse_args()

    # Validate new version format
    try:
        parse_version(args.new_version)
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

    new_version = args.new_version

    print("=" * 70)
    print("Vox Populi Complete Release Script")
    print("=" * 70)

    # =========================================================================
    # PHASE 1: Preparation (Steps 1-8)
    # =========================================================================
    print("\n" + "=" * 70)
    print("PHASE 1: Preparation (Steps 1-8)")
    print("=" * 70)

    # Step 1: Check git status
    print("\n[STEP 1] Checking git status...")
    if not args.skip_git_check:
        check_git_status()
    else:
        print("  Skipped git check")

    # Get current version
    print("\nGetting current version...")
    current_version = get_current_version_from_iss()
    current_mod_version = get_current_mod_version()
    print(f"  Current display version: {current_version}")
    print(f"  Current mod version: {current_mod_version}")
    print(f"  New version: {new_version}")

    # Determine savegame compatibility
    savegame_compatible = is_savegame_compatible(current_version, new_version)
    print(f"\n  Savegame compatible: {savegame_compatible}")

    if savegame_compatible:
        new_mod_version = current_mod_version
        print("  (No numeric version increment needed)")
    else:
        new_mod_version = current_mod_version + 1
        print(f"  New mod version will be: {new_mod_version}")

    # Step 2: Create Debug folder structure
    print("\n[STEP 2] Creating Debug folder structure...")
    if args.debug_folder:
        debug_folder = args.debug_folder
    else:
        debug_folder = PROJECT_DIR.parent / "Release_Debug"
    standard_folder, civ43_folder = create_debug_folder(debug_folder)

    # Step 3: Update version numbers if savegame incompatible
    print("\n[STEP 3] Updating version numbers...")
    if not savegame_compatible:
        print("  Updating numeric versions (savegame incompatible release)...")
        update_mod_version_numbers(current_mod_version, new_mod_version)
        update_dependency_min_versions(current_mod_version, new_mod_version)
        update_custom_mods_h(current_mod_version, new_mod_version)
    else:
        print("  Skipping numeric version updates (savegame compatible)")

    # Step 4: Update displayed version numbers
    print("\n[STEP 4] Updating displayed version numbers...")
    update_all_teaser_versions(current_version, new_version)
    update_vp_setup_iss(current_version, new_version)
    update_bug_report_yml(current_version, new_version)

    if not args.skip_build:
        # Step 5: Create release commit and tag
        print("\n[STEP 5] Creating release commit and tag...")
        create_release_commit_and_tag(new_version)

        # Step 6: Build standard DLLs
        print("\n[STEP 6] Building standard DLLs...")
        print("  Building Release configuration...")
        if not build_dll('release'):
            raise RuntimeError("Release build failed")
        copy_release_dll_to_mod(is_43_civ=False)

        print("\n  Building Debug configuration...")
        if not build_dll('debug'):
            raise RuntimeError("Debug build failed")
        copy_dll_and_pdb('debug', standard_folder)

        print("\n  Cleaning up build folders...")
        cleanup_build_folders()

        # Step 7: Build 43 Civ DLLs
        print("\n[STEP 7] Building 43 Civ DLLs...")
        enable_43_civ_build()
        undo_commit_and_tag(new_version)
        create_release_commit_and_tag(new_version)

        print("  Building Release configuration (43 Civ)...")
        if not build_dll('release'):
            raise RuntimeError("43 Civ Release build failed")
        copy_release_dll_to_mod(is_43_civ=True)

        print("\n  Building Debug configuration (43 Civ)...")
        if not build_dll('debug'):
            raise RuntimeError("43 Civ Debug build failed")
        copy_dll_and_pdb('debug', civ43_folder)

        disable_43_civ_build()

        print("\n  Cleaning up build folders...")
        cleanup_build_folders()

        print(f"\n  Debug folder ready at: {debug_folder}")
        print("  Please compress this folder to a .zip file manually")

        # Step 8: Undo commit and tag
        print("\n[STEP 8] Undoing commit and tag...")
        undo_commit_and_tag(new_version)
    else:
        print("\n[SKIPPING STEPS 5-8] --skip-build flag was set")

    # =========================================================================
    # PHASE 2: Mod Building (Steps 9-10)
    # =========================================================================
    print("\n" + "=" * 70)
    print("PHASE 2: Mod Building (Steps 9-10)")
    print("=" * 70)

    build_mods()

    # =========================================================================
    # PHASE 3: Finalization (Steps 11-12)
    # =========================================================================
    print("\n" + "=" * 70)
    print("PHASE 3: Finalization (Steps 11-12)")
    print("=" * 70)

    # Step 11: Compile autoinstaller
    print("\n[STEP 11] Compiling autoinstaller...")
    if not args.skip_compile:
        compile_installer()
    else:
        print("  Skipped (--skip-compile)")


    # Step 12: Create release commit and tag
    print("\n[STEP 12] Creating final release commit and tag...")
    create_release_commit_and_tag(new_version)

    # =========================================================================
    # Completion
    # =========================================================================
    print("\n" + "=" * 70)
    print("Release process complete!")
    print("=" * 70)
    print("\nRemaining manual steps:")
    print("  1. Push to GitHub")
    print(f"    git push origin master")
    print(f"    git push origin Release-{new_version}")
    print("  2. Compress the Debug folder to a .zip file")
    print("  3. Go to GitHub Releases and draft a new release")
    print(f"  4. Select the tag: Release-{new_version}")
    print("  5. Upload the autoinstaller and Debug.zip")
    print("  6. Generate release notes and edit as needed")
    print("  7. Publish the release")
    print("  8. Update CivFanatics forum thread")
    print("  9. Announce on Discord")


if __name__ == '__main__':
    main()
