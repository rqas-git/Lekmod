














#ifndef FLuaTypeExposure_h
#define FLuaTypeExposure_h
#pragma once

#include "FLuaCommon.h"



#if defined(LEKMOD_MACOS)
#define FLUA_EXPOSE_TYPE_EX(CLASS, NAME) \
	static FLua::TypeExposure<CLASS>::NameRegistrar LuaRegTypeName_##NAME(#NAME);
#else
#define FLUA_EXPOSE_TYPE_EX(CLASS, NAME) \
	static FLua::TypeExposure<CLASS>::NameRegistrar LuaRegTypeName_##NAME##(#NAME);
#endif



#define FLUA_EXPOSE_TYPE(CLASS) FLUA_EXPOSE_TYPE_EX(CLASS, CLASS)




#define FLUA_EXPOSE_INHERITANCE_EX(CHILD, PARENT, REGISTRAR_NAME) \
	static FLua::TypeExposure<CHILD>::ParentRegistrar<PARENT> LuaReg_Inheritance_##REGISTRAR_NAME



#define FLUA_EXPOSE_INHERITANCE(CHILD, PARENT) FLUA_EXPOSE_INHERITANCE_EX(CHILD, PARENT, ##PARENT##_##CHILD##)





#if defined(LEKMOD_MACOS)
#define FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, CATEGORY_FLAGS) \
	int LuaThunk_##CLASS##_##THUNK_NAME(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #THUNK_NAME); \
	} \
	int LuaToString_##CLASS##_##THUNK_NAME(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #THUNK_NAME, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<CLASS>::FunctionRegistrar LuaReg_##CLASS##_##THUNK_NAME \
	(#THUNK_NAME, &LuaThunk_##CLASS##_##THUNK_NAME, &LuaToString_##CLASS##_##THUNK_NAME, FLua::Details::DoConstCheck(&CLASS::FUNC), CATEGORY_FLAGS)
#else
#define FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, CATEGORY_FLAGS) \
	int LuaThunk_##CLASS##_##THUNK_NAME##(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #THUNK_NAME); \
	} \
	int LuaToString_##CLASS##_##THUNK_NAME##(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #THUNK_NAME, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<##CLASS##>::FunctionRegistrar LuaReg_##CLASS##_##THUNK_NAME## \
	(#THUNK_NAME, &LuaThunk_##CLASS##_##THUNK_NAME##, &LuaToString_##CLASS##_##THUNK_NAME##, FLua::Details::DoConstCheck(&CLASS::FUNC), CATEGORY_FLAGS)
#endif




#define FLUA_EXPOSE_MEMBER_EX(CLASS, FUNC, THUNK_NAME) FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, FLua::TypeExposures::sm_uiUnrestrictedExposure)




#define FLUA_EXPOSE_MEMBER_R(CLASS, FUNC, CATEGORY_FLAGS) FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, FUNC, CATEGORY_FLAGS)



#define FLUA_EXPOSE_MEMBER(CLASS, FUNC) FLUA_EXPOSE_MEMBER_EX(CLASS, FUNC, FUNC)




#if defined(LEKMOD_MACOS)
#define FLUA_EXPOSE_MEMBER_AS_NONCONST(CLASS, FUNC) \
	int LuaThunk_##CLASS##_##FUNC(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #FUNC); \
	} \
	int LuaToString_##CLASS##_##FUNC(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #FUNC, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<CLASS>::FunctionRegistrar LuaReg_##CLASS##_##FUNC \
	(#FUNC, &LuaThunk_##CLASS##_##FUNC, &LuaToString_##CLASS##_##FUNC, false, FLua::TypeExposures::sm_uiUnrestrictedExposure)
#else
#define FLUA_EXPOSE_MEMBER_AS_NONCONST(CLASS, FUNC) \
	int LuaThunk_##CLASS##_##FUNC##(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #FUNC); \
	} \
	int LuaToString_##CLASS##_##FUNC##(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #FUNC, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<##CLASS##>::FunctionRegistrar LuaReg_##CLASS##_##FUNC## \
	(#FUNC, &LuaThunk_##CLASS##_##FUNC##, &LuaToString_##CLASS##_##FUNC##, false, FLua::TypeExposures::sm_uiUnrestrictedExposure)
#endif



#define FLUA_SUPPORT_ENUM(ENUM) \
	namespace FLua { \
	namespace Details { \
	template<> static inline ENUM Get(lua_State *L, int idx) { return (ENUM)lua_tointeger(L, idx); } \
	static inline void Push(lua_State *L, const ENUM &eVal) { lua_pushinteger(L, eVal); } \
	template<> static const char *DescribeType<ENUM>() { return #ENUM; } \
		} \
	}

namespace FLua
{

	class TypeExposures
	{
	public:
		static const unsigned int sm_uiMaxTypeExposures = 1024;
		static const unsigned int sm_uiUnrestrictedExposure = (unsigned int)-1;

		static void Register(lua_State *L, unsigned int uiCategoryFlags)
		{
			TypeExposures::PushTypesTable(L);
			TypeExposures::PushMembersTable(L);

			TypeExposures &kRef = Ref();
			for( size_t i = 0; i < kRef.m_uiTypeEntryCount; ++i )
				(*kRef.m_aTypeEntries[i])(L, uiCategoryFlags);

			lua_pop(L, 2);
		}

		static void PushTypesTable(lua_State *L) {
			lua_getglobal(L, "ExposedTypes");
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 1);
				lua_newtable(L);


				lua_pushvalue(L, -1);
				lua_setglobal(L, "ExposedTypes");


				lua_newtable(L);
				lua_pushcfunction(L, TypeToString);
				lua_setfield(L, -2, "__tostring");
				lua_rawseti(L, -2, 1);
			}
		}

		static void PushMembersTable(lua_State *L) {
			lua_getglobal(L, "ExposedMembers");
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 1);
				lua_newtable(L);


				lua_pushvalue(L, -1);
				lua_setglobal(L, "ExposedMembers");
			}
		}

		static int DefaultToString(lua_State *L) {
			if( lua_getmetatable(L, -1) ) {
				lua_getfield(L, -1, "CTypeName");
				lua_getfield(L, -3, "CData");
				Details::CData kCData = (Details::CData)lua_touserdata(L, -1);
				lua_pop(L, 1);
				char szPtr[32];
				sprintf_s(szPtr, 32, ": %p", kCData.GetPtr());
				lua_pushstring(L, szPtr);
				lua_concat(L, 2);
				lua_remove(L, -2);
			}
			else {
				Details::Error("Missing metatable for table representing a C Object.");
				lua_pushstring(L, "Malformed C Reflection");
			}

			return 1;
		}


		static int TypeToString(lua_State *L) {
			lua_pushstring(L, "Metatable for type: ");
			lua_getfield(L, -2, "CTypeName");
			if( !lua_isstring(L, -1) )
			{
				lua_pop(L, 1);
				lua_pushstring(L, "Unknown");
			}
			lua_concat(L, 2);
			return 1;
		}

		static int Equals(lua_State *L) {
			bool bEqual = false;
			if( lua_istable(L, 1) && lua_istable(L, 2) ) {

				lua_getfield(L, 1, "CData");
				lua_getfield(L, 2, "CData");
				if( lua_isnil(L, 3) ) {
					if( lua_isnil(L, 4) )
						bEqual = lua_topointer(L, 1) == lua_topointer(L, 2);
				}
				else {
					Details::CData kCDataLhs = (Details::CData)lua_touserdata(L, 3);
					Details::CData kCDataRhs = (Details::CData)lua_touserdata(L, 4);
					bEqual = kCDataLhs.GetPtr() == kCDataRhs.GetPtr();
				}
				lua_pop(L, 2);
			}

			lua_pushboolean(L, bEqual);
			return 1;
		}








		static int IndexFunction(lua_State *L) {

			PushMembersTable(L);
			if( lua_getmetatable(L, -3) != 0 )
			{
				bool bContinue = true;
				do 
				{
					lua_pushvalue(L, -1);
					lua_rawget(L, -3);
					if( lua_istable(L, -1) )
					{
						lua_pushvalue(L, -4);
						lua_rawget(L, -2);
						if( !lua_isnil(L, -1) )
						{
							lua_insert(L, -4);
							lua_pop(L, 3);
							return 1;
						}
						else
						{
							lua_pop(L, 2);
						}
					}
					else
					{
						lua_pop(L, 1);
					}


					lua_getfield(L, -1, "BaseType");
					if( lua_istable(L, -1) )
					{
						lua_remove(L, -2);
					}
					else
					{
						lua_pop(L, 3);
						bContinue = false;
					}
				} while (bContinue);
			}
			else
			{
				lua_pop(L, 1);
			}
			lua_pushnil(L);
			return 1;
		}

		typedef void (*ExposeTypeFn)(lua_State *L, unsigned int uiCategoryFlags);
		static void AddExposedType(ExposeTypeFn pfnExposeFn)
		{
			TypeExposures &kRef = Ref();


			assert(kRef.m_uiTypeEntryCount < sm_uiMaxTypeExposures && "Not enough space for exposing types");
			if( kRef.m_uiTypeEntryCount < sm_uiMaxTypeExposures )
				kRef.m_aTypeEntries[kRef.m_uiTypeEntryCount++] = pfnExposeFn;
		}

	private:
		TypeExposures() : m_uiTypeEntryCount(0) {}
		static inline TypeExposures &Ref() { static TypeExposures kInst; return kInst; }

		ExposeTypeFn m_aTypeEntries[sm_uiMaxTypeExposures];
		size_t m_uiTypeEntryCount;
	};


	template<class T>
	class TypeExposure
	{
	public:

		static const unsigned int sm_uiMaxFunctions = 64;


		struct NameRegistrar {
			inline NameRegistrar(const char *szName) {
				Ref().m_szName = szName;
				sprintf_s(Ref().m_szConstName, 64, "const %s", szName);

				Details::TypeNameHolder<T>::Get() = szName;
				Details::TypeNameHolder<const T>::Get() = Ref().m_szConstName;
			}
		};


		template<class TParent>
		struct ParentRegistrar {
			inline ParentRegistrar(){
				TypeExposure &kRef = Ref();
				kRef.m_pfnPushParentMetatable = TypeExposure<TParent>::PushMetaTable;
				kRef.m_pfnPushConstParentMetatable = TypeExposure<TParent>::PushConstMetaTable;
			}
		};


		struct FunctionRegistrar {
			inline FunctionRegistrar(const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, bool bIsConst, unsigned int uiCategoryFlags) {
				TypeExposure &kRef = Ref();


				assert(kRef.m_uiFuncEntryCount < sm_uiMaxFunctions && "Not enough space for exposing functions");


				kRef.m_aFuncEntries[kRef.m_uiFuncEntryCount++].Set(szName, pfnThunk, pfnToString, bIsConst, uiCategoryFlags);
			}
		};


		static void PushMetaTable(lua_State *L)
		{
			TypeExposure &kRef = Ref();


			TypeExposures::PushTypesTable(L);
			lua_getfield(L, -1, kRef.m_szName);
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 2);
				kRef.CreateMetaTable(L);
			}
			else
				lua_remove(L, -2);
		}


		static void PushConstMetaTable(lua_State *L)
		{
			TypeExposure &kRef = Ref();


			TypeExposures::PushTypesTable(L);
			lua_getfield(L, -1, kRef.m_szConstName);
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 2);
				kRef.CreateConstMetaTable(L);
			}
			else
				lua_remove(L, -2);
		}

	private:
		struct FuncEntry
		{
			inline void Set(const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, bool bConstFunc, unsigned int uiCategoryFlags)
			{
				m_szName = szName;
				m_pfnThunk = pfnThunk;
				m_pfnToString = pfnToString;
				m_bConstFunc = bConstFunc;
				m_uiCategoryFlags = uiCategoryFlags;
			}

			const char *m_szName;
			unsigned int m_uiCategoryFlags;
			lua_CFunction m_pfnThunk;
			lua_CFunction m_pfnToString;
			bool m_bConstFunc;
		};

		TypeExposure() :
			m_uiFuncEntryCount(0), m_pfnPushParentMetatable(NULL), m_pfnPushConstParentMetatable(NULL)
		{ 
			m_szName = "Unkown";
			Details::GetPushMetaTableFn<T>() = &PushMetaTable;
			Details::GetPushMetaTableFn<const T>() = &PushConstMetaTable;

			TypeExposures::AddExposedType(&TypeExposure::ExposeMembers);
		}

		static inline TypeExposure &Ref() { static TypeExposure kInst; return kInst; }


		void CreateMetaTable(lua_State *L)
		{

			lua_newtable(L);


			lua_pushstring(L, m_szName);
			lua_setfield(L, -2, "CTypeName");


			lua_pushcfunction(L, &TypeExposures::DefaultToString);
			lua_setfield(L, -2, "__tostring");


			lua_pushcfunction(L, &TypeExposures::Equals);
			lua_setfield(L, -2, "__eq");


			lua_pushcfunction(L, TypeExposures::IndexFunction);
			lua_setfield(L, -2, "__index");


			if( m_pfnPushParentMetatable )
			{
				(*m_pfnPushParentMetatable)(L);
				lua_setfield(L, -2, "BaseType");
			}

			TypeExposures::PushTypesTable(L);


			lua_rawgeti(L, -1, 1);
			if( lua_istable(L, -1) )
			{
				lua_setmetatable(L, -3);
			}
			else
			{
				lua_pop(L, 1);
			}

			lua_pushvalue(L, -2);
			lua_setfield(L, -2, m_szName);
			lua_pop(L, 1);
		}


		void CreateTypeMemberTable(lua_State *L)
		{
			lua_newtable(L);
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				lua_pushcfunction(L, kEntry.m_pfnThunk);
				Details::SetToStringForFunc(L, kEntry.m_pfnToString);
				lua_setfield(L, -2, kEntry.m_szName);
			}
		}

		void CreateTypeMemberTable(lua_State *L, unsigned int uiCategoryFlags)
		{
			lua_newtable(L);
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_uiCategoryFlags == TypeExposures::sm_uiUnrestrictedExposure || (kEntry.m_uiCategoryFlags & uiCategoryFlags) != 0 )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}


		void CreateConstMetaTable(lua_State *L)
		{

			lua_newtable(L);


			lua_pushstring(L, m_szConstName);
			lua_setfield(L, -2, "CTypeName");


			lua_pushcfunction(L, &TypeExposures::DefaultToString);
			lua_setfield(L, -2, "__tostring");


			lua_pushcfunction(L, &TypeExposures::Equals);
			lua_setfield(L, -2, "__eq");


			lua_pushcfunction(L, TypeExposures::IndexFunction);
			lua_setfield(L, -2, "__index");


			if( m_pfnPushConstParentMetatable )
			{
				(*m_pfnPushConstParentMetatable)(L);
				lua_setfield(L, -2, "BaseType");
			}

			TypeExposures::PushTypesTable(L);


			lua_rawgeti(L, -1, 1);
			if( lua_istable(L, -1) )
			{
				lua_setmetatable(L, -3);
			}
			else
			{
				lua_pop(L, 1);
			}

			lua_pushvalue(L, -2);
			lua_setfield(L, -2, m_szConstName);
			lua_pop(L, 1);
		}


		void CreateConstTypeMemberTable(lua_State *L)
		{
			lua_newtable(L);
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_bConstFunc )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}

		void CreateConstTypeMemberTable(lua_State *L, unsigned int uiCategoryFlags)
		{
			lua_newtable(L);
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_bConstFunc && (kEntry.m_uiCategoryFlags == TypeExposures::sm_uiUnrestrictedExposure || (kEntry.m_uiCategoryFlags & uiCategoryFlags) != 0) )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}



		static void ExposeMembers(lua_State *L, unsigned int uiCategoryFlags)
		{
			TypeExposure &kRef = Ref();


			lua_getfield(L, -2, kRef.m_szName);
			if( !lua_istable(L, -1) )
			{
				lua_pop(L, 1);
				kRef.CreateMetaTable(L);
			}

			kRef.CreateTypeMemberTable(L);
			lua_rawset(L, -3);


			lua_getfield(L, -2, kRef.m_szConstName);
			if( !lua_istable(L, -1) )
			{
				lua_pop(L, 1);
				kRef.CreateConstMetaTable(L);
			}

			kRef.CreateConstTypeMemberTable(L);
			lua_rawset(L, -3);
		}

		const char *m_szName;
		char m_szConstName[64];
		FuncEntry m_aFuncEntries[sm_uiMaxFunctions];
		size_t m_uiFuncEntryCount;

		Details::PushMetaTableFn m_pfnPushParentMetatable;
		Details::PushMetaTableFn m_pfnPushConstParentMetatable;
	};

	namespace Details
	{

		template<class TClass>
		static int Call(lua_CFunction pfn, lua_State *L, const TCHAR *szLuaFnName) {
			return pfn(L);
		}




		template<class TClass, class TFuncOwner>
		static int Call(void(TFuncOwner::*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			if( lua_gettop(L) == 1 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) 
				{
					FLUA_SAVECALLSTACK(L);
					(pkObj->*pfn)();
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet>
		static int Call(TRet(TFuncOwner::*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			if( lua_gettop(L) == 1 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					TRet ret = (pkObj->*pfn)();
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0>
		static int Call(void(TFuncOwner::*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
									  FromLuaAnalog<TArg2>::Convert(arg2));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
												 FromLuaAnalog<TArg2>::Convert(arg2));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
									  FromLuaAnalog<TArg3>::Convert(arg3));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
												 FromLuaAnalog<TArg3>::Convert(arg3));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3),
									  FromLuaAnalog<TArg4>::Convert(arg4));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3),
												 FromLuaAnalog<TArg4>::Convert(arg4));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}




		template<class TClass, class TFuncOwner>
		static int Call(void(TFuncOwner::*pfn)()const, lua_State *L, const TCHAR *szLuaFnName) {
			if( lua_gettop(L) == 1 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) 
				{
					FLUA_SAVECALLSTACK(L);
					(pkObj->*pfn)();
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet>
		static int Call(TRet(TFuncOwner::*pfn)()const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			if( lua_gettop(L) == 1 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					TRet ret = (pkObj->*pfn)();
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0>
		static int Call(void(TFuncOwner::*pfn)(TArg0)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                          FromLuaAnalog<TArg2>::Convert(arg2));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3),
						              FromLuaAnalog<TArg4>::Convert(arg4));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}


		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3),
						                         FromLuaAnalog<TArg4>::Convert(arg4));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}




		template<class TRet, class TFuncOwner>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( void )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName);
		}


		template<class TRet, class TFuncOwner, class TArg0>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get(),
				DescribeType<TArg4>(), PtrDescription<TArg4>::Get());
		}




		template<class TRet, class TFuncOwner>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)()const, const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( void ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName);
		}


		template<class TRet, class TFuncOwner, class TArg0>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0)const, const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get());
		}


		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get(),
				DescribeType<TArg4>(), PtrDescription<TArg4>::Get());
		}
	}
}

#endif
