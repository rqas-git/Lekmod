











#ifndef _INC_BANNED
#	define _INC_BANNED
#endif

#ifdef _MSC_VER

#	pragma once
#	pragma deprecated (strcpy, strcpyA, strcpyW, wcscpy, _tcscpy, _mbscpy, StrCpy, StrCpyA, StrCpyW, lstrcpy, lstrcpyA, lstrcpyW, _tccpy, _mbccpy)
#	pragma deprecated (strcat, strcatA, strcatW, wcscat, _tcscat, _mbscat, StrCat, StrCatA, StrCatW, lstrcat, lstrcatA, lstrcatW, StrCatBuff, StrCatBuffA, StrCatBuffW, StrCatChainW, _tccat, _mbccat)
#	pragma deprecated (wnsprintf, wnsprintfA, wnsprintfW, sprintfW, sprintfA, wsprintf, wsprintfW, wsprintfA, sprintf, swprintf, _stprintf, _snwprintf, _snprintf, _sntprintf)
#	pragma deprecated (wvsprintf, wvsprintfA, wvsprintfW, vsprintf, _vstprintf, vswprintf)
#	pragma deprecated (_vsnprintf, _vsnwprintf, _vsntprintf, wvnsprintf, wvnsprintfA, wvnsprintfW)
#	pragma deprecated (strncpy, wcsncpy, _tcsncpy, _mbsncpy, _mbsnbcpy, StrCpyN, StrCpyNA, StrCpyNW, StrNCpy, strcpynA, StrNCpyA, StrNCpyW, lstrcpyn, lstrcpynA, lstrcpynW)
#	pragma deprecated (strncat, wcsncat, _tcsncat, _mbsncat, _mbsnbcat, StrCatN, StrCatNA, StrCatNW, StrNCat, StrNCatA, StrNCatW, lstrncat, lstrcatnA, lstrcatnW, lstrcatn)
#	pragma deprecated (strtok, _tcstok, wcstok, _mbstok)
#	pragma deprecated (makepath, _tmakepath,  _makepath, _wmakepath)
#	pragma deprecated (_splitpath, _tsplitpath, _wsplitpath)
#	pragma deprecated (scanf, wscanf, _tscanf, sscanf, swscanf, _stscanf, snscanf, snwscanf, _sntscanf)

#	pragma deprecated (gets, _getts, _gettws)
#	pragma deprecated (IsBadWritePtr, IsBadHugeWritePtr, IsBadReadPtr, IsBadHugeReadPtr, IsBadCodePtr, IsBadStringPtr)
#	pragma deprecated (CharToOem, CharToOemA, CharToOemW, OemToChar, OemToCharA, OemToCharW, CharToOemBuffA, CharToOemBuffW)

#	pragma deprecated (strlen, wcslen, _mbslen, _mbstrlen, StrLen, lstrlen)
#	pragma deprecated (memcpy, RtlCopyMemory, CopyMemory)
#else
#ifdef __GNUC__

#	pragma GCC poison strcpy strcpyA strcpyW wcscpy _tcscpy _mbscpy StrCpy StrCpyA StrCpyW lstrcpy lstrcpyA lstrcpyW _tccpy _mbccpy
#	pragma GCC poison strcat strcatA strcatW wcscat _tcscat _mbscat StrCat StrCatA StrCatW lstrcat lstrcatA lstrcatW StrCatBuff StrCatBuffA StrCatBuffW StrCatChainW _tccat _mbccat
#	pragma GCC poison wnsprintf wnsprintfA wnsprintfW sprintfW sprintfA wsprintf wsprintfW wsprintfA sprintf swprintf _stprintf _snwprintf _snprintf _sntprintf
#	pragma GCC poison wvsprintf wvsprintfA wvsprintfW vsprintf _vstprintf vswprintf
#	pragma GCC poison _vsnprintf _vsnwprintf _vsntprintf wvnsprintf wvnsprintfA wvnsprintfW
#	pragma GCC poison strncpy wcsncpy _tcsncpy _mbsncpy _mbsnbcpy StrCpyN StrCpyNA StrCpyNW StrNCpy strcpynA StrNCpyA StrNCpyW lstrcpyn lstrcpynA lstrcpynW
#	pragma GCC poison strncat wcsncat _tcsncat _mbsncat _mbsnbcat StrCatN StrCatNA StrCatNW StrNCat StrNCatA StrNCatW lstrncat lstrcatnA lstrcatnW lstrcatn
#	pragma GCC poison strtok _tcstok wcstok _mbstok
#	pragma GCC poison makepath _tmakepath  _makepath _wmakepath
#	pragma GCC poison _splitpath _tsplitpath _wsplitpath
#	pragma GCC poison scanf wscanf _tscanf sscanf swscanf _stscanf snscanf snwscanf _sntscanf

#	pragma GCC poison gets _getts _gettws
#	pragma GCC poison IsBadWritePtr IsBadHugeWritePtr IsBadReadPtr IsBadHugeReadPtr IsBadCodePtr IsBadStringPtr
#	pragma GCC poison CharToOem CharToOemA CharToOemW OemToChar OemToCharA OemToCharW CharToOemBuffA CharToOemBuffW

#	pragma GCC poison strlen wcslen _mbslen _mbstrlen StrLen lstrlen
#	pragma GCC poison memcpy RtlCopyMemory CopyMemory
#endif

#endif
