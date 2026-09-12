




















#ifndef FLuaPointerProtection_h
#define FLuaPointerProtection_h
#pragma once

#include "FLuaCommon.h"
#include "FLuaTypes.h"



#define FLUA_POINTER_PROTECTION_MEMBER() FLua::Details::PointerProtection FLuaPointerProtection



#define FLUA_TYPE_USES_POINTER_PROTECTION(TYPE) \
	namespace FLua{ namespace Details{ \
	static void PushCData(lua_State *L, TYPE *pVal) { \
		if( pVal ) pVal->FLuaPointerProtection.PushCData(L, (void*)pVal); \
		else { \
			lua_pushlightuserdata(L, pVal); \
		}\
	} \
	}}

namespace FLua
{
	namespace Details
	{
		class PointerProtection
		{
		public:
			PointerProtection();
			~PointerProtection();


			void PushCData(lua_State *L, void *pVal);

		private:
			struct UserDataNode
			{
				UserDataNode() : m_pkNext(NULL) {}
				Value m_kUD;
				UserDataNode *m_pkNext;
			};

			UserDataNode m_kUserData;
		};
	}
}

#endif
