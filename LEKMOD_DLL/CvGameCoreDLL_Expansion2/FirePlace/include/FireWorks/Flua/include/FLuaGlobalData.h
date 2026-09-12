











#ifndef FLuaGlobalData_h
#define FLuaGlobalData_h
#pragma once

#include "FLuaCommon.h"



#define FLUA_EXPOSE_GLOBAL_DATA(DATA_PTR, NAME) \
	static FLua::GlobalData::Registrar LuaReg_Data_##NAME##(#NAME, (##DATA_PTR##))

namespace FLua
{
	class GlobalData
	{
	public:
		static const unsigned int sm_uiMaxDataEntries = 16;

		struct Registrar {
			template<class T>
			inline Registrar( _In_z_ const char *szName, T *pData) {
				GlobalData &kRef = Ref();


				assert(kRef.m_uiDataEntryCount < sm_uiMaxDataEntries && "Not enough space for exposing global data");


				kRef.m_aDataEntries[kRef.m_uiDataEntryCount++].Set(szName, pData);
			}
		};


		static void Register(lua_State *L) {
			GlobalData &kRef = Ref();
			for( size_t i = 0; i < kRef.m_uiDataEntryCount; ++i ) {
				const DataEntry &kEntry = kRef.m_aDataEntries[i];
				(*kEntry.m_pfnAddGlobal)(L, kEntry.m_pvData, kEntry.m_szName);
			}
		}

	private:
		struct DataEntry
		{
			typedef void(AddGlobalFn)(lua_State*, void*, const char*);
			template<class T>
			static void AddGlobal(lua_State *L, void *pvData, const char *szName) {
				Details::Push(L, (T*)pvData);
				lua_setglobal(L, szName);
			}

			template<class T>
			inline void Set(const char *szName, T *pData) {
				m_szName = szName;
				m_pvData = (void*)pData;
				m_pfnAddGlobal = &DataEntry::AddGlobal<T>;
			}

			const char *m_szName;
			void *m_pvData;
			AddGlobalFn *m_pfnAddGlobal;
		};

		GlobalData() : m_uiDataEntryCount(0) {}
		static inline GlobalData &Ref() { static GlobalData kInst; return kInst; }

		DataEntry m_aDataEntries[sm_uiMaxDataEntries];
		size_t m_uiDataEntryCount;
	};
}

#endif
