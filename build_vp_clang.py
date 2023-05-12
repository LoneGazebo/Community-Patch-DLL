import os
import subprocess
from enum import Enum
import typing
import time
import tempfile
from pathlib import Path
import argparse
from queue import Queue

class Config(Enum):
    Release = 0
    Debug = 1

VS_2008_VARS_BAT = Path(os.environ['VS90COMNTOOLS']).joinpath('vsvars32.bat')
CORE_DLL = 'CvGameCore_Expansion2'
PROJECT_DIR = Path().resolve()
BUILD_DIR = {
    Config.Release: 'clang-build\\Release',
    Config.Debug: 'clang-build\\Debug',
}
OUT_DIR = {
    Config.Release: 'clang-output\\Release',
    Config.Debug: 'clang-output\\Debug',
}
LIBS = [
    'CvWorldBuilderMap\\lib\\CvWorldBuilderMapWin32.obj',
    'CvGameCoreDLLUtil\\lib\\CvGameCoreDLLUtilWin32.lib',
    'CvLocalization\\lib\\CvLocalizationWin32.lib',
    'CvGameDatabase\\lib\\CvGameDatabaseWin32.lib',
    'FirePlace\\lib\\FireWorksWin32.obj',
    'FirePlace\\lib\\FLuaWin32.lib',
    'ThirdPartyLibs\\Lua51\\lib\\lua51_Win32.lib',
]
DEFAULT_LIBS = [
    'winmm.lib',
    'kernel32.lib',
    'user32.lib',
    'gdi32.lib',
    'winspool.lib',
    'comdlg32.lib',
    'advapi32.lib',
    'shell32.lib',
    'ole32.lib',
    'oleaut32.lib',
    'uuid.lib',
    'odbc32.lib',
    'odbccp32.lib',
]
DEF_FILE = 'CvGameCoreDLL_Expansion2\\CvGameCoreDLL.def'
INCLUDE_DIRS = [
    'CvGameCoreDLL_Expansion2',
    'CvWorldBuilderMap\\include',
    'CvGameCoreDLLUtil\\include',
    'CvLocalization\\include',
    'CvGameDatabase\\include',
    'FirePlace\\include',
    'FirePlace\\include\\FireWorks',
    'ThirdPartyLibs\\Lua51\\include'
]
SHARED_PREDEFS = [
    'FXS_IS_DLL',
    'WIN32',
    '_WINDOWS',
    '_USRDLL',
    'EXTERNAL_PAUSING',
    'STACKWALKER',
    'CVGAMECOREDLL_EXPORTS',
    'FINAL_RELEASE',
    '_CRT_SECURE_NO_WARNINGS',
    '_WINDLL',
]
RELEASE_PREDEFS = SHARED_PREDEFS + ['STRONG_ASSUMPTIONS', 'NDEBUG']
DEBUG_PREDEFS = SHARED_PREDEFS + ['VPDEBUG']
PREDEFS = {
    Config.Release: RELEASE_PREDEFS,
    Config.Debug: DEBUG_PREDEFS,
}
CL_SUPPRESS = [
    'invalid-offsetof',
    'tautological-constant-out-of-range-compare',
    'comment',
    'enum-constexpr-conversion', # TODO: #9786
]
PCH_CPP = 'CvGameCoreDLL_Expansion2\\_precompile.cpp'
PCH_H = 'CvGameCoreDLLPCH.h'
PCH = 'CvGameCoreDLLPCH.pch'
CPP = [
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaArea.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaArgsHandle.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaCity.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaDeal.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaEnums.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaFractal.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaGame.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaGameInfo.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaLeague.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaMap.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaPlayer.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaPlot.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaSupport.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaTeam.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaTeamTech.cpp',
    'CvGameCoreDLL_Expansion2\\Lua\\CvLuaUnit.cpp',
    'CvGameCoreDLL_Expansion2\\stackwalker\\StackWalker.cpp',
    'CvGameCoreDLL_Expansion2\\CustomMods.cpp',
    'CvGameCoreDLL_Expansion2\\CvAchievementInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvAchievementUnlocker.cpp',
    'CvGameCoreDLL_Expansion2\\CvAdvisorCounsel.cpp',
    'CvGameCoreDLL_Expansion2\\CvAdvisorRecommender.cpp',
    'CvGameCoreDLL_Expansion2\\CvAIOperation.cpp',
    'CvGameCoreDLL_Expansion2\\CvArea.cpp',
    'CvGameCoreDLL_Expansion2\\CvArmyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvAStar.cpp',
    'CvGameCoreDLL_Expansion2\\CvAStarNode.cpp',
    'CvGameCoreDLL_Expansion2\\CvBarbarians.cpp',
    'CvGameCoreDLL_Expansion2\\CvBeliefClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvBuilderTaskingAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvBuildingClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvBuildingProductionAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvCity.cpp',
    'CvGameCoreDLL_Expansion2\\CvCityAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvCityCitizens.cpp',
    'CvGameCoreDLL_Expansion2\\CvCityConnections.cpp',
    'CvGameCoreDLL_Expansion2\\CvCityManager.cpp',
    'CvGameCoreDLL_Expansion2\\CvCitySpecializationAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvCityStrategyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvContractClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvCorporationClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvCultureClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvDangerPlots.cpp',
    'CvGameCoreDLL_Expansion2\\CvDatabaseUtility.cpp',
    'CvGameCoreDLL_Expansion2\\CvDealAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvDealClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvDiplomacyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvDiplomacyRequests.cpp',
    'CvGameCoreDLL_Expansion2\\CvDistanceMap.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllBuildInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllBuildingInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllCity.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllCivilizationInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllColorInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllCombatInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllContext.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllDatabaseUtility.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllDeal.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllDealAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllDiplomacyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllDlcPackageInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllEraInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllFeatureInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllGame.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllGameAsynch.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllGameDeals.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllGameOptionInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllGameSpeedInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllHandicapInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllImprovementInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllInterfaceModeInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllLeaderheadInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllMap.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllMinorCivInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllMissionData.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllMissionInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllNetInitInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllNetLoadGameInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllNetMessageExt.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllNetMessageHandler.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllNetworkSyncronization.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPathFinderUpdate.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPlayer.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPlayerColorInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPlayerOptionInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPlot.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPolicyInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPreGame.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllPromotionInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllRandom.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllResourceInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllScriptSystemUtility.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllTeam.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllTechInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllTerrainInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllUnit.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllUnitCombatClassInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllUnitInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllVictoryInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllWorldBuilderMapLoader.cpp',
    'CvGameCoreDLL_Expansion2\\CvDllWorldInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvEconomicAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvEmphasisClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvEspionageClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvEventLog.cpp',
    'CvGameCoreDLL_Expansion2\\CvFlavorManager.cpp',
    'CvGameCoreDLL_Expansion2\\CvFractal.cpp',
    'CvGameCoreDLL_Expansion2\\CvGame.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameCoreDLL.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameCoreEnumSerialization.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameCoreStructs.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameCoreUtils.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameQueries.cpp',
    'CvGameCoreDLL_Expansion2\\CvGameTextMgr.cpp',
    'CvGameCoreDLL_Expansion2\\CvGlobals.cpp',
    'CvGameCoreDLL_Expansion2\\CvGoodyHuts.cpp',
    'CvGameCoreDLL_Expansion2\\CvGrandStrategyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvGreatPersonInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvHomelandAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvImprovementClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvInfos.cpp',
    'CvGameCoreDLL_Expansion2\\CvInfosSerializationHelper.cpp',
    'CvGameCoreDLL_Expansion2\\CvInternalGameCoreUtils.cpp',
    'CvGameCoreDLL_Expansion2\\CvLoggerCSV.cpp',
    'CvGameCoreDLL_Expansion2\\CvMap.cpp',
    'CvGameCoreDLL_Expansion2\\CvMapGenerator.cpp',
    'CvGameCoreDLL_Expansion2\\CvMilitaryAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvMinorCivAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvNotificationClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvNotifications.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlayer.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlayerAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlayerManager.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlot.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlotInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvPlotManager.cpp',
    'CvGameCoreDLL_Expansion2\\CvPolicyAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvPolicyClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvPopupInfoSerialization.cpp',
    'CvGameCoreDLL_Expansion2\\CvPreGame.cpp',
    'CvGameCoreDLL_Expansion2\\CvProcessProductionAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvProjectClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvProjectProductionAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvPromotionClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvRandom.cpp',
    'CvGameCoreDLL_Expansion2\\CvReligionClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvReplayInfo.cpp',
    'CvGameCoreDLL_Expansion2\\CvReplayMessage.cpp',
    'CvGameCoreDLL_Expansion2\\CvSerialize.cpp',
    'CvGameCoreDLL_Expansion2\\CvSiteEvaluationClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvStartPositioner.cpp',
    'CvGameCoreDLL_Expansion2\\cvStopWatch.cpp',
    'CvGameCoreDLL_Expansion2\\CvTacticalAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvTacticalAnalysisMap.cpp',
    'CvGameCoreDLL_Expansion2\\CvTargeting.cpp',
    'CvGameCoreDLL_Expansion2\\CvTeam.cpp',
    'CvGameCoreDLL_Expansion2\\CvTechAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvTechClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvTradeClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvTraitClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvTreasury.cpp',
    'CvGameCoreDLL_Expansion2\\CvTypes.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnit.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitCombat.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitCycler.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitMission.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitMovement.cpp',
    'CvGameCoreDLL_Expansion2\\CvUnitProductionAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvVotingClasses.cpp',
    'CvGameCoreDLL_Expansion2\\CvWonderProductionAI.cpp',
    'CvGameCoreDLL_Expansion2\\CvWorldBuilderMapLoader.cpp',
]

class TaskResult:
    commands: typing.Union[str, list[str]]
    returncode: int
    
    def __init__(self, commands: typing.Union[str, list[str]]):
        self.commands = commands
        self.returncode = None

class Task:
    proc: subprocess.Popen
    result: TaskResult
    def __init__(self, commands: typing.Union[str, list[str]], env: typing.Optional[dict[str, str]]=None, shell: bool=False, log:any =None):
        self.proc = subprocess.Popen(commands, stdout=log, stderr=log, env=env, shell=shell)
        self.result = TaskResult(commands)

    def poll(self) -> typing.Optional[TaskResult]:
        if (returncode := self.proc.poll()) != None:
            self.result.returncode = returncode
            return self.result
        else:
            return None

class TaskMan:
    pending: Queue
    def __init__(self):
        self.pending = Queue()

    def spawn(self, commands: typing.Union[str, list[str]], env: typing.Optional[dict[str, str]]=None, shell: bool=False, log:any =None):
        task = Task(commands, env=env, shell=shell, log=log)
        self.pending.put(task)

    def wait(self) -> list[TaskResult]:
        results: list[TaskResult] = []
        while not self.pending.empty():
            task = self.pending.get()
            if result := task.poll():
                results.append(result)
            else:
                self.pending.put(task)
        return results

def build_cl_config_args(config: Config) -> list[str]:
    args = ['-m32', '-msse3', '/c', '/MD', '/GS', '/EHsc', '/fp:precise', '/Zc:wchar_t', '/Z7']
    if config == Config.Release:
        args.append('/Ox')
        args.append('/Ob2')
        args.append('-flto')
    else:
        args.append('/Od')
    for predef in PREDEFS[config]:
        args.append(f'/D{predef}')
    for include_dir in INCLUDE_DIRS:
        args.append(f'/I"{os.path.join(PROJECT_DIR, include_dir)}"')
    for suppress in CL_SUPPRESS:
        args.append(f'-Wno-{suppress}')
    return args

def build_link_config_args(config: Config) -> list[str]:
    args = ['/MACHINE:x86', '/DLL', '/DEBUG', '/LTCG', '/DYNAMICBASE', '/NXCOMPAT', '/SUBSYSTEM:WINDOWS', '/MANIFEST:EMBED', f'/DEF:"{os.path.join(PROJECT_DIR, DEF_FILE)}"']
    if config == Config.Release:
        args += ['/OPT:REF', '/OPT:ICF']
    return args

def prepare_dirs(build_dir: Path, out_dir: Path):
    build_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)
    for cpp in CPP:
        cpp_dir = build_dir.joinpath(Path(cpp).parent)
        cpp_dir.mkdir(parents=True, exist_ok=True)

def build_clang_cpp(cl: str, cl_args: str, build_dir: Path, log: typing.IO):
    print('building clang.cpp...')
    start_time = time.time()
    src = PROJECT_DIR.joinpath('clang.cpp')
    out = build_dir.joinpath('clang.obj')
    command = f'"{VS_2008_VARS_BAT}">NUL && {cl} "{src}" /Fo"{out}" {cl_args}'
    cp = subprocess.run(command, capture_output=True)
    log.write(str.encode(f'==== {src} ====\n'))
    log.write(cp.stdout)
    log.write(cp.stderr)
    log.flush()
    if cp.returncode != 0:
        print('failed to build clang.cpp - see build log')
        quit()
    end_time = time.time()
    print(f'clang.cpp build finished after {end_time - start_time} seconds')

def update_commit_id(log: typing.IO):
    print('updating commit id...')
    start_time = time.time()
    cp = subprocess.run('update_commit_id.bat', capture_output=True)
    log.write(str.encode(f'==== update_commit_id.bat ====\n'))
    log.write(cp.stdout)
    log.write(cp.stderr)
    log.flush()
    if cp.returncode != 0:
        print('failed to update commit id - see build log')
        quit()
    end_time = time.time()
    print(f'commit id update finished after {end_time - start_time} seconds')

def build_pch(cl: str, cl_args: str, pch_path: Path, build_dir: Path, log: typing.IO):
    print('building precompiled header...')
    start_time = time.time()
    pch_src = PROJECT_DIR.joinpath(PCH_CPP)
    out = build_dir.joinpath(PCH_CPP).with_suffix('.obj')
    command = f'"{VS_2008_VARS_BAT}">NUL && {cl} "{pch_src}" /Fo"{out}" /Yc"{PCH_H}" /Fp"{pch_path}" {cl_args}'
    cp = subprocess.run(command, capture_output=True)
    log.write(str.encode(f'==== {pch_src} ====\n'))
    log.write(cp.stdout)
    log.write(cp.stderr)
    log.flush()
    if cp.returncode != 0:
        print('failed to build precompiled header - see build log')
        quit()
    end_time = time.time()
    print(f'precompiled header build finished after {end_time - start_time} seconds')

def build_cpps(cl: str, cl_args: str, pch_path: Path, build_dir: Path, log: typing.IO):
    print('building cpps...')
    start_time = time.time()
    build_tasks = TaskMan()
    logs: dict[Path, typing.IO] = {}
    try:
        for cpp in CPP:
            cpp_src = PROJECT_DIR.joinpath(cpp)
            cpp_log = tempfile.TemporaryFile()
            logs[cpp_src] = cpp_log
            out = build_dir.joinpath(cpp).with_suffix('.obj')
            command = f'"{VS_2008_VARS_BAT}">NUL && {cl} "{cpp_src}" /Fo"{out}" /Yu"{PCH_H}" /Fp"{pch_path}" {cl_args}'
            build_tasks.spawn(command, log=cpp_log)
        build_results = build_tasks.wait()
        for cpp_src, cpp_log in logs.items():
            cpp_log.seek(0, 0)
            contents = cpp_log.read();
            log.write(str.encode(f'==== {cpp_src} ====\n'))
            log.write(contents)
            del cpp_log
        log.flush()
        failed = 0
        for result in build_results:
            if result.returncode != 0:
                failed += 1
        if failed != 0:
            print(f'{failed} cpp(s) failed to build - see build log')
            quit()
        end_time = time.time()
        print(f'cpps build finished after {end_time - start_time} seconds')
    finally:
       del logs

def link_dll(link: str, link_args: list[str], build_dir: Path, out_dir: Path, log: typing.IO):
    print('linking dll...')
    start_time = time.time()
    link_response_file_name = build_dir.joinpath('link')
    with open(link_response_file_name, 'w') as link_response_file:
        out_dll = out_dir.joinpath(f'{CORE_DLL}.dll')
        out_pdb = out_dir.joinpath(f'{CORE_DLL}.pdb')
        link_response_file.write(f'/OUT:"{out_dll}"\n/PDB:"{out_pdb}"\n')
        link_response_file.write('\n'.join(link_args))
        for lib in LIBS:
            lib_path = PROJECT_DIR.joinpath(lib)
            link_response_file.write(f'\n"{lib_path}"')
        for default_lib in DEFAULT_LIBS:
            link_response_file.write(f'\n"{default_lib}"')
        clang_obj = build_dir.joinpath('clang.obj')
        pch_obj = build_dir.joinpath(PCH_CPP).with_suffix('.obj')
        link_response_file.write(f'\n"{clang_obj}"\n"{pch_obj}"')
        for cpp in CPP:
            cpp_obj = build_dir.joinpath(cpp).with_suffix('.obj')
            link_response_file.write(f'\n"{cpp_obj}"')
        link_response_file.close()
    command = f'"{VS_2008_VARS_BAT}">NUL && {link} @"{link_response_file_name}"'
    cp = subprocess.run(command, capture_output=True)
    log.write(str.encode(f'==== {CORE_DLL}.dll ====\n'))
    log.write(cp.stdout)
    log.write(cp.stderr)
    log.flush()
    end_time = time.time()
    if cp.returncode != 0:
        print('linking dll failed - see build log')
        quit()
    print(f'linking dll finished after {end_time - start_time} seconds')

arg_parser = argparse.ArgumentParser(description='Build VP.')
arg_parser.add_argument('--config', type=str, default='debug', choices=['release', 'debug'])
args = arg_parser.parse_args()
config = Config.Release if args.config == 'release' else Config.Debug

cl = 'clang-cl.exe'
link = 'lld-link.exe'
build_dir = PROJECT_DIR.joinpath(BUILD_DIR[config])
out_dir = PROJECT_DIR.joinpath(PROJECT_DIR, OUT_DIR[config])
cl_args = ' '.join(build_cl_config_args(config))
link_args = build_link_config_args(config)
pch_path = os.path.join(build_dir, PCH)
prepare_dirs(build_dir, out_dir)

log = open(out_dir.joinpath('build.log'), mode='w+b')
try:
    update_commit_id(log)
    build_clang_cpp(cl, cl_args, build_dir, log)
    build_pch(cl, cl_args, pch_path, build_dir, log)
    build_cpps(cl, cl_args, pch_path, build_dir, log)
    link_dll(link, link_args, build_dir, out_dir, log)
finally:
    log.close()