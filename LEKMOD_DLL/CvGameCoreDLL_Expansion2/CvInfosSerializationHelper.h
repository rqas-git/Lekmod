






#ifndef CVINFOSSERIALIZATIONHELPER_H
#define CVINFOSSERIALIZATIONHELPER_H

#pragma once







namespace CvInfosSerializationHelper
{

template<typename TData>
void ReadV0DataArray(FDataStream& kStream, TData* paArray, int iArraySize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiV0TagCount; iI++)
	{
		TData tValue;
		kStream >> tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			if(iType < iArraySize)
				paArray[iType] = tValue;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}
}


template<typename TData>
void ReadV0DataArray(FDataStream& kStream, std::vector<TData>& aiArray, uint uiMaxSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(aiArray.size() < uiMaxSize)
		aiArray.resize(uiMaxSize);

	uint uiSize;
	kStream >> uiSize;

	for(uint iI = 0; iI < uiV0TagCount && iI < uiSize; iI++)
	{
		TData tValue;
		kStream >> tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			aiArray[iType] = tValue;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}
}


template<typename TData>
void ReadV0DataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiV0TagCount; iI++)
	{
		TData tValue;
		int iType = GC.getInfoTypeForString(ppszV0Tags[iI]);
		if(iType != -1)
		{
			if(iType < iArraySize)
			{
				TData* paiArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paiArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %s", ppszV0Tags[iI]);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);


				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
					kStream >> tValue;
			}
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %s", ppszV0Tags[iI]);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);

			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}


template<typename TType>
void ReadV0TypeArray(FDataStream& kStream, std::vector<TType>& aiArray, uint uiMaxSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(aiArray.size() < uiMaxSize)
		aiArray.resize(uiMaxSize);

	uint uiSize;
	kStream >> uiSize;

	for(uint iI = 0; iI < uiSize; iI++)
	{
		TType eType;
		kStream >> eType;

		if(eType == (TType)-1)
			aiArray[iI] = (TType)-1;
		else
		{
			if((uint) eType < uiV0TagCount)
			{
				int iMappedType = GC.getInfoTypeForString(ppszV0Tags[eType]);
				if(iMappedType != -1)
				{
					aiArray[iI] = (TType)iMappedType;
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Type not found: %d", eType);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
			else
				aiArray[iI] = (TType)-1;
		}
	}
}



template<typename TType>
void ReadV0TypeArray(FDataStream& kStream, TType* paArray, uint uiSize, const char** ppszV0Tags, uint uiV0TagCount)
{
	for(uint iI = 0; iI < uiSize; iI++)
	{
		TType eType;
		kStream >> eType;

		if(eType == (TType)-1)
			paArray[iI] = (TType)-1;
		else
		{
			if((uint) eType < uiV0TagCount)
			{
				int iMappedType = GC.getInfoTypeForString(ppszV0Tags[eType]);
				if(iMappedType != -1)
				{
					paArray[iI] = (TType)iMappedType;
				}
				else
				{
					CvString szError;
					szError.Format("LOAD ERROR: Type not found: %d", eType);
					GC.LogMessage(szError.GetCString());
					CvAssertMsg(false, szError);
				}
			}
			else
				paArray[iI] = (TType)-1;
		}
	}
}


template<typename TType>
TType ConvertV0(TType eType, const char** ppszV0Tags, uint uiV0TagCount)
{
	if(eType != (TType)-1 && (uint)eType < uiV0TagCount)
	{
		int iMappedType = GC.getInfoTypeForString(ppszV0Tags[(int)eType]);
		if(iMappedType != -1)
		{
			return (TType)iMappedType;
		}
		else
		{
			CvString szError;
			szError.Format("LOAD ERROR: Type not found: %d", (int)eType);
			GC.LogMessage(szError.GetCString());
			CvAssertMsg(false, szError);
		}
	}

	return (TType)-1;
}


template<typename TData>
void ReadDataArray(FDataStream& kStream, TData* paArray, int iArraySize)
{
	uint uiNumEntries;
	FStringFixedBuffer(sTemp, 256);
	int iType;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		if(iType != -1)
		{
			TData tValue;
			kStream >> tValue;
			if((int)iType < iArraySize)
				paArray[iType] = tValue;
		}
		else if(!bValid)
		{
			TData tValue;
			kStream >> tValue;
		}
	}
}


template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, TData* paArray, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		if(iType != -1)
		{
			TData tValue;
			kStream >> tValue;
			if((int)iType < iArraySize)
				paArray[iType] = tValue;
		}
		else if(!bValid)
		{
			TData tValue;
			kStream >> tValue;
		}
	}
}


template<typename TData>
void ReadDataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		TData tValue;
		if(iType != -1)
		{
			if((int)iType < iArraySize)
			{
				int* paArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %d", iType);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bValid = false;
			}

		}

		if(!bValid)
		{
			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}


template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, TData** ppaArray, int iSubArraySize, int iArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		TData tValue;
		if(iType != -1)
		{
			if((int)iType < iArraySize)
			{
				int* paArray = ppaArray[iType];
				for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				{
					kStream >> tValue;
					paArray[iJ] = tValue;
				}
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Type mapping outside the supplied array bounds: %d", iType);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bValid = false;
			}
		}

		if(!bValid)
		{
			for(int iJ = 0; iJ < iSubArraySize; ++iJ)
				kStream >> tValue;
		}
	}
}


template<typename TData>
void ReadDataArray(FDataStream& kStream, std::vector<TData>& aiArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aiArray.size() < uiNumEntries)
		aiArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = Read(kStream, &bValid);
		if(iType != -1)
		{
			if (iType >= (int)aiArray.size())
				aiArray.resize(iType+1);

			kStream >> aiArray[iType];
		}
		else if(!bValid)
		{
			TData tDummy;
			kStream >> tDummy;
		}
	}
}


template<typename TData>
void ReadHashedDataArray(FDataStream& kStream, std::vector<TData>& aiArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aiArray.size() < uiNumEntries)
		aiArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		bool bValid;
		int iType = ReadHashed(kStream, &bValid);
		if(iType != -1)
		{
			if (iType >= (int)aiArray.size())
				aiArray.resize(iType+1);

			kStream >> aiArray[iType];
		}
		else if(!bValid)
		{
			TData tDummy;
			kStream >> tDummy;
		}
	}
}


template<typename TType>
void ReadTypeArray(FDataStream& kStream, std::vector<TType>& aArray)
{
	uint uiNumEntries;
	int iType;

	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)Read(kStream);
	}
}


template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, std::vector<TType>& aArray)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)ReadHashed(kStream);
	}
}


template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, TType* paArray, uint uiArraySize)
{
	uint uiNumEntries;

	kStream >> uiNumEntries;

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		TType eType = (TType)ReadHashed(kStream);
		if(iI < uiArraySize)
			paArray[iI] = eType;
	}
}



template<typename TType>
void ReadHashedTypeArray(FDataStream& kStream, uint uiInputArraySize, TType* paArray, uint uiOutputArraySize)
{
	for(uint iI = 0; iI < uiInputArraySize; iI++)
	{
		TType eType = (TType)ReadHashed(kStream);
		if(iI < uiOutputArraySize)
			paArray[iI] = eType;
	}
}



template<typename TType>
bool ReadAndRemapDataArray(FDataStream& kStream, int iSrcCount, TType* pvDest, int iDestCount, const int* piaRemap)
{
	TType* pvBuffer = (TType*)_malloca(iSrcCount * sizeof(TType));
#ifdef AUI_WARNING_FIXES
	if (!pvBuffer)
		return false;
#endif
	kStream.ReadIt(iSrcCount * sizeof(TType), (void*)pvBuffer);

	bool bRemapSuccess = true;
	for(int i = 0; i < iSrcCount; ++i)
	{
		int iDestIndex = piaRemap[i];
		if(iDestIndex >= 0 && iDestIndex < iDestCount)
		{
			pvDest[iDestIndex] = pvBuffer[i];
		}
		else
			bRemapSuccess = false;
	}
	_freea(pvBuffer);
	return bRemapSuccess;
}


template<typename TType>
void ReadTypeArrayDBLookup(FDataStream& kStream, std::vector<TType>& aArray, const char* szTable)
{
	uint uiNumEntries;
	kStream >> uiNumEntries;

	if(aArray.size() < uiNumEntries)
		aArray.resize(uiNumEntries);

	for(uint iI = 0; iI < uiNumEntries; iI++)
	{
		aArray[iI] = (TType)ReadDBLookup(kStream, szTable);
	}
}




int Read(FDataStream& kStream, bool* bValid = NULL);


int ReadHashed(FDataStream& kStream, bool* bValid = NULL);


int ReadDBLookup(FDataStream& kStream, const char* szTable, bool* bValid = NULL);







template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, TData* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(Write(kStream, eType))
		{
			kStream << paArray[iI];
		}
	}
}


template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, TData* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(WriteHashed(kStream, eType))
		{
			kStream << paArray[iI];
		}
	}
}


template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, const std::vector<TData>& aArray)
{
	kStream << aArray.size();

	for(uint iI = 0; iI < aArray.size(); iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(Write(kStream, eType))
		{
			kStream << aArray[iI];
		}
	}
}


template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, const std::vector<TData>& aArray)
{
	kStream << aArray.size();

	for(uint iI = 0; iI < aArray.size(); iI++)
	{
		const TType eType = static_cast<TType>(iI);
		if(WriteHashed(kStream, eType))
		{
			kStream << aArray[iI];
		}
	}
}

#ifdef LEKMOD_UNITCOMBAT_FREE_PROMOTION


template<typename TType, typename TData>
void WriteHashed2DDataArray(FDataStream& kStream, TData** ppaArray, uint uiSubArraySize, uint uiArraySize)
{
	kStream << uiArraySize;

	for (uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);

		if (WriteHashed(kStream, eType))
		{
			TData* paArray = ppaArray[iI];
			for (uint iJ = 0; iJ < uiSubArraySize; ++iJ)
			{
				kStream << paArray[iJ];
			}
		}
	}
}
#endif


template<typename TType, typename TData>
void WriteDataArray(FDataStream& kStream, TData** ppaArray, uint uiSubArraySize, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);

		if(Write(kStream, eType))
		{
			TData* paArray = ppaArray[iI];
			for(uint iJ = 0; iJ < uiSubArraySize; ++iJ)
			{
				kStream << paArray[iJ];
			}
		}
	}
}


template<typename TType, typename TData>
void WriteHashedDataArray(FDataStream& kStream, TData** ppaArray, uint uiSubArraySize, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		const TType eType = static_cast<TType>(iI);

		if(WriteHashed(kStream, eType))
		{
			TData* paArray = ppaArray[iI];
			for(uint iJ = 0; iJ < uiSubArraySize; ++iJ)
			{
				kStream << paArray[iJ];
			}
		}
	}
}


template<typename TType>
void WriteTypeArray(FDataStream& kStream, const std::vector<TType>& aiArray)
{
	kStream << aiArray.size();

	for(uint iI = 0; iI < aiArray.size(); iI++)
	{
		Write(kStream, aiArray[iI]);
	}
}


template<typename TType>
void WriteHashedTypeArray(FDataStream& kStream, const std::vector<TType>& aiArray)
{
	kStream << aiArray.size();

	for(uint iI = 0; iI < aiArray.size(); iI++)
	{
		WriteHashed(kStream, aiArray[iI]);
	}
}


template<typename TType>
void WriteHashedTypeArray(FDataStream& kStream, TType* paArray, uint uiArraySize)
{
	kStream << uiArraySize;

	for(uint iI = 0; iI < uiArraySize; iI++)
	{
		WriteHashed(kStream, paArray[iI]);
	}
}

#define CVINFO_V0_TAG_COUNT(x)	(sizeof(x) / sizeof(const char*))
#define CVINFO_V0_TAGS(x)	&x[0], sizeof(x) / sizeof(const char*)


#define DECLARE_SERIALIZATION_INFO_TYPE_HELPER(theType) \
extern bool Write(FDataStream& kStream, const theType eType); \
extern bool WriteHashed(FDataStream& kStream, const theType eType);


#define IMPLEMENT_SERIALIZATION_INFO_TYPE_HELPER(theType, theInfoAccess, theNoEnum) \
bool Write(FDataStream& kStream, const theType eType) \
{ \
	if(eType != theNoEnum) \
		return Write(kStream, (const CvBaseInfo*)GC.theInfoAccess(eType)); \
	else \
		return Write(kStream, (const CvBaseInfo*)NULL); \
} \
bool WriteHashed(FDataStream& kStream, const theType eType) \
{ \
	if(eType != theNoEnum) \
		return WriteHashed(kStream, (const CvBaseInfo*)GC.theInfoAccess(eType)); \
	else \
		return WriteHashed(kStream, (const CvBaseInfo*)NULL); \
}








extern const char* ms_V0ResourceTags[27];
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(ResourceTypes);
extern const char* ms_V0PolicyTags[60];
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(PolicyTypes);
extern const char* ms_V0PolicyBranchTags[10];
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(PolicyBranchTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(TechTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(BeliefTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(ReligionTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(PlayerColorTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(MinorCivTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(SpecialistTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(EmphasizeTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(ProjectTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(VoteTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(VoteSourceTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(SpecialUnitTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(VictoryTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(BuildingTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(PromotionTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(UnitTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(FeatureTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(TerrainTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(AICityStrategyTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(SmallAwardTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(RouteTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(BuildTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(ProcessTypes);
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(ImprovementTypes);
#if defined(TRAITIFY)
DECLARE_SERIALIZATION_INFO_TYPE_HELPER(BuildingClassTypes);
#endif







bool Write(FDataStream& kStream, const CvBaseInfo* pkInfo);

bool WriteHashed(FDataStream& kStream, const CvBaseInfo* pkInfo);
}

#endif
