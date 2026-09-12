"""Focused checks for the minimal follow-up optimizations."""
from pathlib import Path
import subprocess

PREFIX = 'LEKMOD_DLL/CvGameCoreDLL_Expansion2/'
BASE = '2742e9d29bcf184d41d31272b0e5f07f18dfecf1'


def cpp_checks(directory, source, extract, compile_run):
    def method(text, signature):
        # Some signatures are followed by #endif before the opening brace.
        result = extract(text, signature)
        return result[:result.index('{')].replace('#endif\n', '') + result[result.index('{'):]
    result = {}
    astar = source(PREFIX+'CvAStar.cpp')
    header = source(PREFIX+'CvAStar.h')
    first = astar.index('void CvAStar::CreateChildren(')
    second = astar.index('void CvAStar::CreateChildren(', first+1)
    neighbors = {
        'X_RANGE': method(header, 'inline int CvAStar::xRange(int iX)\n'),
        'Y_RANGE': method(header, 'inline int CvAStar::yRange(int iY)\n'),
        'IS_VALID': method(header, 'inline bool CvAStar::isValid(int iX, int iY)\n'),
        'PRECOMPUTE': extract(astar, 'void CvAStar::PrecalcNeighbors('),
        'CACHED': extract(astar[first:], 'void CvAStar::CreateChildren('),
        'BASELINE': extract(astar[second:], 'void CvAStar::CreateChildren(').replace('::CreateChildren(', '::CreateChildrenBaseline('),
    }
    result['neighbors'] = compile_run('neighbors.cpp.in', neighbors, directory, ('-fsanitize=undefined', '-fno-sanitize-recover=all'))
    return result
