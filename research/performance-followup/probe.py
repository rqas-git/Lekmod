
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
CORE = ROOT/'LEKMOD_DLL/CvGameCoreDLL_Expansion2'
BASE = '2742e9d29bcf184d41d31272b0e5f07f18dfecf1'

def read(name):
    return (CORE/name).read_text(errors='replace')

def baseline(name):
    return subprocess.check_output(['git','show',f'{BASE}:LEKMOD_DLL/CvGameCoreDLL_Expansion2/{name}'],cwd=ROOT).decode(errors='replace')

def extract(text, signature):
    start=text.index(signature);end=text.index('{',start)+1;depth=1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}');end+=1
    return text[start:end]

tech=baseline('CvTechAI.cpp')
old=extract(tech,'float CvTechAI::GetTechRatio()')
linear=old.replace('CvTechAI::GetTechRatio()', 'CvTechAI::LinearTechRatio()')
linear=linear.replace('std::vector<LeaderWithNumTechs> aLeaderWithNumTechs;', 'LeaderWithNumTechs aLeaderWithNumTechs[MAX_MAJOR_CIVS];\n\tuint uiLeaderCount = 0;')
linear=linear.replace('aLeaderWithNumTechs.push_back(kEvalChunk);','aLeaderWithNumTechs[uiLeaderCount++] = kEvalChunk;')
linear=linear.replace('std::stable_sort(aLeaderWithNumTechs.begin(), aLeaderWithNumTechs.end(), LeaderWithNumTechsEval());','')
linear=linear.replace('aLeaderWithNumTechs.size()', 'uiLeaderCount')
linear=linear.replace('iPlayerIndexInList = ui;', '''iPlayerIndexInList = 0;
            for (uint other=0; other<uiLeaderCount; ++other)
            {
                if (aLeaderWithNumTechs[other].m_iNumTechs > aLeaderWithNumTechs[ui].m_iNumTechs ||
                    (aLeaderWithNumTechs[other].m_iNumTechs == aLeaderWithNumTechs[ui].m_iNumTechs && other < ui))
                    ++iPlayerIndexInList;
            }''')
mission=extract(baseline('CvPlayerAI.cpp'),'int CvPlayerAI::AI_plotTargetMissionAIs(')
boolean=mission.replace('int CvPlayerAI::AI_plotTargetMissionAIs(', 'bool CvPlayerAI::HasPlotTargetMissionAI(')
boolean=boolean.replace('int iCount = 0;', '').replace('iCount++;','return true;').replace('return iCount;', 'return false;')
template=(HERE/'selection.cpp.in').read_text()
for key,value in {'TECH_BASELINE':old,'TECH_LINEAR':linear,'MISSION_BASELINE':mission,'MISSION_BOOLEAN':boolean,
    'TECH_STRUCTS':tech[tech.index('struct LeaderWithNumTechs\n'):tech.index('float CvTechAI::GetTechRatio()')]}.items():
    template=template.replace('@'+key+'@',value)

command=json.loads((ROOT/'macos/build/CvAStar.command.json').read_text())
flags=command[:command.index('-c')]
preprocessed=subprocess.run(flags+['-dM','-E','-x','c++','-'],input='#include "CvGameCoreDLLPCH.h"\n',text=True,capture_output=True,check=True)
defines={m.group(1):m.group(2).strip() for m in re.finditer(r'^#define (\w+)(.*)$',preprocessed.stdout,re.M)}
names=['FINAL_RELEASE','AI_PERF_LOGGING','AUI_WARNING_FIXES',
       'AUI_WORKER_FIX_SHOULD_BUILDER_CONSIDER_PLOT_EXISTING_BUILD_MISSIONS_SHIFT',
       'AUI_WORKER_SHOULD_BUILDER_CONSIDER_PLOT_MAXIMUM_DANGER_BASED_ON_UNIT_STRENGTH',
       'LEKMOD_NO_INSTANT_REPAIR_ON_ROUTE']
result={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
        'prototype_baseline':BASE,
        'active_macros':{n:defines.get(n) for n in names},
        'compiler':subprocess.check_output(['clang++','--version'],text=True).splitlines()[0]}
with tempfile.TemporaryDirectory(prefix='lekmod-followup-') as tmp:
    file=Path(tmp)/'selection.cpp';file.write_text(template)
    for mode,extra in [('default',[]),('ubsan',['-fsanitize=undefined','-fno-sanitize-recover=all'])]:
        binary=Path(tmp)/mode
        subprocess.run(['clang++','-arch','x86_64','-std=c++14','-O2','-fwrapv','-ffp-contract=off',*extra,str(file),'-o',str(binary)],check=True)
        result[mode]=json.loads(subprocess.check_output([str(binary)],text=True,timeout=60))
    assert result['default']==result['ubsan']
paths=['CvTechAI.cpp','CvPlayerAI.cpp','CvBuilderTaskingAI.cpp','CvCityStrategyAI.cpp','CvCityStrategyAI.h',
       'CvInfosSerializationHelper.cpp','CvInfos.cpp','CvInfos.h','CvPlot.cpp','CvMap.cpp','CvUnit.cpp',
       'CvDatabaseUtility.cpp','CvBuildingClasses.cpp','CvBuildingClasses.h','CvEconomicAI.cpp',
       'FirePlace/include/FireWorks/FStringA.inl']
result['source_sha256']={str((CORE/p).relative_to(ROOT)):hashlib.sha256((CORE/p).read_bytes()).hexdigest() for p in paths}
result['fixture_sha256']={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest()
                          for p in (HERE/'probe.py',HERE/'selection.cpp.in')}
print(json.dumps(result,indent=2))
