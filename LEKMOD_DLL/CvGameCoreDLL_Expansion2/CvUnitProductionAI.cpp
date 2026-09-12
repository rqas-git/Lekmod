






#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "CvUnitProductionAI.h"
#include "CvGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"

#include "LintFree.h"



CvUnitProductionAI::CvUnitProductionAI(CvCity* pCity,  CvUnitXMLEntries* pUnits):
	m_pCity(pCity),
	m_pUnits(pUnits)
{
}


CvUnitProductionAI::~CvUnitProductionAI(void)
{
}


void CvUnitProductionAI::Reset()
{
	CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");

	m_UnitAIWeights.clear();


	if(m_pUnits)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pUnits->GetNumUnits(); i++)
#else
		for(int i = 0; i < m_pUnits->GetNumUnits(); i++)
#endif
		{
			m_UnitAIWeights.push_back(i, 0);
		}
	}
}


void CvUnitProductionAI::Read(FDataStream& kStream)
{

	uint uiVersion;
	kStream >> uiVersion;

	int iWeight;


	m_UnitAIWeights.clear();


	if(m_pUnits)
	{
#ifdef AUI_WARNING_FIXES
		for (uint i = 0; i < m_pUnits->GetNumUnits(); i++)
#else
		for(int i = 0; i < m_pUnits->GetNumUnits(); i++)
#endif
		{
			m_UnitAIWeights.push_back(i, 0);
		}

#ifdef AUI_WARNING_FIXES
		uint iNumEntries;
		int iType;

		kStream >> iNumEntries;

		for (uint iI = 0; iI < iNumEntries; iI++)
#else
		int iNumEntries;
		int iType;

		kStream >> iNumEntries;

		for(int iI = 0; iI < iNumEntries; iI++)
#endif
		{
			bool bValid = true;
			iType = CvInfosSerializationHelper::ReadHashed(kStream, &bValid);
			if(iType != -1 || !bValid)
			{
				kStream >> iWeight;
				if(iType != -1)
				{
					m_UnitAIWeights.IncreaseWeight(iType, iWeight);
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Unit Type not found");
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
		}
	}
	else
	{
		CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
}


void CvUnitProductionAI::Write(FDataStream& kStream) const
{
	FStringFixedBuffer(sTemp, 64);


	uint uiVersion = 1;
	kStream << uiVersion;

	if(m_pUnits)
	{
#ifdef AUI_WARNING_FIXES
		uint iNumUnits = m_pUnits->GetNumUnits();
		kStream << iNumUnits;


		for (uint iI = 0; iI < iNumUnits; iI++)
#else
		int iNumUnits = m_pUnits->GetNumUnits();
		kStream << iNumUnits;


		for(int iI = 0; iI < iNumUnits; iI++)
#endif
		{
			const UnitTypes eUnit = static_cast<UnitTypes>(iI);
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if(pkUnitInfo)
			{
				CvInfosSerializationHelper::WriteHashed(kStream, pkUnitInfo);
				kStream << m_UnitAIWeights.GetWeight(iI);
			}
			else
			{
				kStream << (int)0;
			}
		}
	}
	else
	{
		CvAssertMsg(m_pUnits != NULL, "Unit Production AI init failure: unit entries are NULL");
	}
}


void CvUnitProductionAI::AddFlavorWeights(FlavorTypes eFlavor, int iWeight)
{

#ifdef AUI_WARNING_FIXES
	for (uint iUnit = 0; iUnit < m_pUnits->GetNumUnits(); iUnit++)
#else
	for(int iUnit = 0; iUnit < m_pUnits->GetNumUnits(); iUnit++)
#endif
	{
		CvUnitEntry* entry = m_pUnits->GetEntry(iUnit);
		if(entry)
		{

			m_UnitAIWeights.IncreaseWeight(iUnit, entry->GetFlavorValue(eFlavor) * iWeight);
		}
	}
}


int CvUnitProductionAI::GetWeight(UnitTypes eUnit)
{
	return m_UnitAIWeights.GetWeight(eUnit);
}


UnitTypes CvUnitProductionAI::RecommendUnit(UnitAITypes eUnitAIType)
{
#ifdef AUI_WARNING_FIXES
	uint iUnitLoop;
#else
	int iUnitLoop;
#endif
	int iWeight;
	int iTurnsLeft;

	if(eUnitAIType <= NO_UNITAI)
	{
		return NO_UNIT;
	}


	m_Buildables.clear();


	for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{

			if(m_pCity->canTrain(eUnit))
			{

				if(eUnitAIType == NO_UNITAI || pkUnitInfo->GetUnitAIType(eUnitAIType))
				{

					iTurnsLeft = m_pCity->getProductionTurnsLeft(eUnit, 0);
					iWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_UnitAIWeights.GetWeight(eUnit), iTurnsLeft);
					m_Buildables.push_back(iUnitLoop, iWeight);
				}
			}
		}

	}


	if(m_Buildables.size() > 0)
	{
		m_Buildables.SortItems();
		LogPossibleBuilds(eUnitAIType);
		return (UnitTypes)m_Buildables.GetElement(0);
	}


	else
	{
		return NO_UNIT;
	}
}


void CvUnitProductionAI::LogPossibleBuilds(UnitAITypes eUnitAIType)
{
	if(GC.getLogging() && GC.getAILogging())
	{

		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();


		FILogFile* pLog = LOGFILEMGR.GetLog(m_pCity->GetCityStrategyAI()->GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);


		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";



		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvString strOutBuf = strBaseString;

			CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(m_Buildables.GetElement(iI));

			CvString strDesc = (pUnitEntry != NULL)? pUnitEntry->GetDescription() : "Unknown Unit";
			if(eUnitAIType == NO_UNITAI)
			{
				CvString strTemp;
				strTemp.Format("Unit, %s, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;
			}
			else
			{
				CvString strTempString;
				getUnitAIString(strTempString, eUnitAIType);

				CvString strTemp;
				strTemp.Format("Special request for unit of type: %s, %s, %d", strTempString.GetCString(), strDesc.GetCString(), m_Buildables.GetWeight(iI));
				strOutBuf += strTemp;
			}

			pLog->Msg(strOutBuf);
		}
	}
}
