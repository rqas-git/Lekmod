






#include "CvGameCoreDLLPCH.h"
#include "CvPopupInfoSerialization.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"


#include "LintFree.h"

FDataStream& operator<<(FDataStream& kStream, const CvPopupInfo& kPopupInfo)
{

	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << kPopupInfo.iData1;
	kStream << kPopupInfo.iData2;
	kStream << kPopupInfo.iData3;
	kStream << kPopupInfo.iFlags;
	kStream << kPopupInfo.bOption1;
	kStream << kPopupInfo.bOption2;

	kStream << kPopupInfo.eButtonPopupType;

	CvString strDummy = kPopupInfo.szText;
	kStream << strDummy;

	return kStream;
}

FDataStream& operator>>(FDataStream& kStream, CvPopupInfo& kPopupInfo)
{

	uint uiVersion;
	kStream >> uiVersion;

	kStream >> kPopupInfo.iData1;
	kStream >> kPopupInfo.iData2;
	kStream >> kPopupInfo.iData3;
	kStream >> kPopupInfo.iFlags;
	kStream >> kPopupInfo.bOption1;
	kStream >> kPopupInfo.bOption2;

	kStream >> kPopupInfo.eButtonPopupType;

	CvString strDummy;
	kStream >> strDummy;

	strcpy_s(kPopupInfo.szText, strDummy.c_str());

	return kStream;
}
