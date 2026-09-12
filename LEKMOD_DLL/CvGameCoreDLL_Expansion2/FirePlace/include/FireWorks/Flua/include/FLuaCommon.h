











#ifndef FLuaCommon_h
#define FLuaCommon_h
#pragma once

#include "lua.hpp"

#include <tchar.h>
#include <stdio.h>
#include <assert.h>

class FCriticalSection;


#if defined(LEKMOD_MACOS)
#define FLUA_COMPILE_TIME_CONDITION(CONDITION, ERR_NAME) typedef int ERROR_##ERR_NAME[(CONDITION)? 1 : -1]
#else
#define FLUA_COMPILE_TIME_CONDITION(CONDITION, ERR_NAME) typedef int ERROR_##ERR_NAME##[(CONDITION)? 1 : -1]
#endif


#define FLUA_COMPILE_TIME_ERROR(ERR_NAME) FLUA_COMPILE_TIME_CONDITION(false, ERR_NAME)

#define FLUA_MULTITHREAD_SUPPORT

namespace FLua
{

	enum Type
	{
		TYPE_NONE = LUA_TNONE,
		TYPE_NIL = LUA_TNIL,
		TYPE_NUMBER = LUA_TNUMBER,
		TYPE_BOOLEAN = LUA_TBOOLEAN,
		TYPE_STRING = LUA_TSTRING,
		TYPE_TABLE = LUA_TTABLE,
		TYPE_FUNCTION = LUA_TFUNCTION,
		TYPE_USERDATA = LUA_TUSERDATA,
		TYPE_LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
		TYPE_THREAD = LUA_TTHREAD,
	};

	namespace Details
	{

		template<class TOut, class TIn>
		TOut UnsafeCast(TIn inVal)
		{
			FLUA_COMPILE_TIME_CONDITION(sizeof(TIn) == sizeof(TOut), CantUseUnsafeCast);
			union { TIn inVal; TOut outVal; } u;
			u.inVal = inVal;
			return u.outVal;
		}

		void LockAccess();
		void UnlockAccess();	
	}


	class ErrorHandler
	{
	public:
		typedef void(*ErrorHandlerStaticFunc)(const TCHAR*);


		ErrorHandler(ErrorHandlerStaticFunc pfn) :
			m_pkClass(NULL),
#if defined(LEKMOD_MACOS)
			m_pfnFunc()
            {
                assert(pfn);
                static_assert(sizeof(pfn) <= sizeof(m_pfnFunc), "function pointer storage");


                memcpy(&m_pfnFunc, &pfn, sizeof(pfn));
            }
#else
			m_pfnFunc(Details::UnsafeCast<ErrorHandlerFunc>(pfn))
			{ assert(pfn); }
#endif
		

		template<class T, class TFuncOwner>
		ErrorHandler(T *p, void(TFuncOwner::*pfn)(const TCHAR*)) :
			m_pkClass(Details::UnsafeCast<ErrorHandlerClass*>(p)),
			m_pfnFunc(Details::UnsafeCast<ErrorHandlerFunc>(pfn))
			{ assert(pfn); }


		template<class T, class TFuncOwner>
		ErrorHandler(const T *p, void(TFuncOwner::*pfn)(const TCHAR*)const) :
			m_pkClass(Details::UnsafeCast<ErrorHandlerClass*>(p)),
			m_pfnFunc(Details::UnsafeCast<ErrorHandlerFunc>(pfn))
			{ assert(pfn); }


		void operator()( _In_z_ const TCHAR *szError) const;

	private:
		class ErrorHandlerClass {};
		typedef void(ErrorHandlerClass::*ErrorHandlerFunc)(const TCHAR*);
		ErrorHandlerClass *m_pkClass;
		ErrorHandlerFunc m_pfnFunc;
	};

	namespace Details {
		static void DefaultErrorHandler(const TCHAR *szError) { fprintf_s(stderr, _T("%s\n"), szError); }
		struct ErrorHandlerHolder { static inline ErrorHandler &Get() { static ErrorHandler kHandler = &DefaultErrorHandler; return kHandler; } };
	}


	static void SetErrorHandler(ErrorHandler kHandler) { Details::ErrorHandlerHolder::Get() = kHandler; }
	static ErrorHandler GetErrorHandler() { return Details::ErrorHandlerHolder::Get(); }


	class ScopedErrorHandler
	{
	public:
		explicit ScopedErrorHandler(ErrorHandler kErrorHandler);
		~ScopedErrorHandler();

	private:
		ErrorHandler m_kPrevErrorHandler;
	};


	_Ret_opt_ lua_State *CreateLuaState();


	_Ret_opt_ lua_State *CreateLuaState(lua_Alloc pfnAllocFn, void *pvUserData);



	bool Execute(lua_State *L, _In_z_ const char *szCode);



	bool LoadFile(lua_State *L, _In_z_ const wchar_t *wszFilename);


	void GetField(lua_State *L, int iTableIndex, const char *szName);


	static inline void GetGlobal(lua_State *L, _In_z_ const char *szName) { GetField(L, LUA_GLOBALSINDEX, szName); }


	void LuaStackToString(lua_State *L, char *szBuffer, size_t uiBufferSize);




	bool ConsoleCommand(lua_State *L, _In_z_ const char *szInput, char *szOutput, size_t uiOutputSize);


	struct FunctionDescription
	{
		FunctionDescription(const char *szFunction, const char *szDescription) :
			szFunction(szFunction), szDescription(szDescription) {}
		FunctionDescription() : szFunction(NULL), szDescription(NULL) {}
		const char *szFunction;
		const char *szDescription;
	};


	bool SetFunctionDescription(lua_State *L, _In_z_ const char *szFunction, _In_z_ const char *szDescription);



	bool SetFunctionDescriptions(lua_State *L, FunctionDescription *akDescriptions);


	class StackValue
	{
	public:
		StackValue(lua_State *L, int iStackIndex);

		inline void Push() const { lua_pushvalue(m_pkLuaState, m_iStackIndex); }
		inline int Reference() const {
			lua_checkstack(m_pkLuaState, 1);
			lua_pushvalue(m_pkLuaState, m_iStackIndex);
			return lua_ref(m_pkLuaState, true);
		}

		inline Type GetType() const { return (Type)lua_type(m_pkLuaState, m_iStackIndex); }
		inline const char *GetTypeName() const { return lua_typename(m_pkLuaState, lua_type(m_pkLuaState, m_iStackIndex)); }

		inline bool IsNil() const { return lua_isnil(m_pkLuaState, m_iStackIndex); }


		inline operator bool() const { return GetAs<bool>(); }
		inline operator int() const { return GetAs<int>(); }
		inline operator char() const { return GetAs<char>(); }
		inline operator unsigned int() const { return GetAs<unsigned int>(); }
		inline operator float() const { return GetAs<float>(); }
		inline operator double() const { return GetAs<double>(); }
		inline operator const char*() const { return GetAs<const char*>(); }
		template<class T> _Ret_opt_ operator T*() const { return GetAs<T*>(); }


		template<class T>
		T GetAs() const {
			T ret = T();


#if defined(LEKMOD_MACOS)
			typedef typename Details::LuaAnalog<T>::Result Analog;
#else
			typedef Details::LuaAnalog<T>::Result Analog;
#endif
			Analog analog = Details::Get<Analog>(m_pkLuaState, m_iStackIndex);


			bool bValid(true); Details::ArgValidator<T>::Validate(analog, bValid);


			if( bValid ) ret = Details::FromLuaAnalog<T>::Convert(analog);

			return ret;
		}


		template<class T>
		bool IsOfType() const { return Details::CompatibleType<T>(m_pkLuaState, m_iStackIndex); }

		inline int GetIndex() const { return m_iStackIndex; }
		inline lua_State *GetLuaState() const { return m_pkLuaState; }

	private:
		lua_State *m_pkLuaState;
		int m_iStackIndex;
	};

	namespace Details
	{

		void Error( _In_z_ const TCHAR *szFormat, ...);
		void Error( lua_State *L, _In_z_ const TCHAR *szFormat, ...);
		const TCHAR *SaveCallStack(lua_State *L);
		const TCHAR* GetLastCallstackBuffer();








		bool CallWithErrorHandling(lua_State *L, unsigned int uiArgCount, unsigned int uiResults);







		bool MultRetCallWithErrorHandling(lua_State *L, unsigned int uiArgCount, unsigned int &uiReturnCountOut);


		template<class T> static T *LuaNew(lua_State *L)
		{
			void *pvUserData = NULL;
			lua_Alloc pfnAlloc = lua_getallocf(L, &pvUserData);
			return new((*pfnAlloc)(pvUserData, NULL, 0, sizeof(T)))T();
		}


		template<class T> void LuaDelete(lua_State *L, T *&p)
		{
			if( p )
			{
				p->~T();
				void *pvUserData = NULL;
				lua_Alloc pfnAlloc = lua_getallocf(L, &pvUserData);
				p = (T*)(*pfnAlloc)(pvUserData, p, sizeof(T), 0);
			}
		}


		template<class T> struct StripPtr    { typedef T Result; };
		template<class T> struct StripPtr<T*>{ typedef T Result; };


		template<class T> struct StripPtrAndRef    { typedef T Result; };
		template<class T> struct StripPtrAndRef<T*>{ typedef T Result; };
		template<class T> struct StripPtrAndRef<T&>{ typedef T Result; };


		template<class T> struct StripConst          { typedef T Result; };
		template<class T> struct StripConst<const T> { typedef T Result; };
		template<class T> struct StripConst<const T*>{ typedef T* Result; };


		template<class T> struct ConstCheck { static const bool bIsConst = false; };
		template<class T> struct ConstCheck<const T> { static const bool bIsConst = true; };
		template<class T> struct ConstCheck<const T*> { static const bool bIsConst = true; };


		template<class T, class TRet> struct ConstCheck<TRet(T::*)()const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0> struct ConstCheck<TRet(T::*)(TArg0)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2, TArg3)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2, TArg3, TArg4)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2, TArg3, TArg4, TArg5)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)const> { static const bool bIsConst = true; };
		template<class T, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6, class TArg7>
		struct ConstCheck<TRet(T::*)(TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)const> { static const bool bIsConst = true; };


		template<class T> inline bool DoConstCheck(T data) { return ConstCheck<T>::bIsConst; };


		class CData {
		public:
			CData(void *pvObject) : m_pvObject(pvObject) {}
			inline void *GetPtr() const { return m_pvObject; }
			inline void NullPtr() { m_pvObject = NULL; }
		private:
			void *m_pvObject;
		};


		typedef void(*PushMetaTableFn)(lua_State *L);


		template<class T> struct MetaTableFnHolder { static PushMetaTableFn pfn; };
		template<class T> PushMetaTableFn MetaTableFnHolder<T>::pfn = NULL;


		template<class T> static inline PushMetaTableFn &GetPushMetaTableFn() { return MetaTableFnHolder<T>::pfn; }


		template<class T> struct LuaAnalog{ typedef T Result; };
		template<class T> struct LuaAnalog<T&>{ typedef T* Result; };
		template<class T> struct LuaAnalog<const T>{ typedef T Result; };
		template<class T> struct LuaAnalog<const T*>{ typedef const T* Result; };
		template<class T> struct LuaAnalog<const T&>{ typedef const T* Result; };
		template<> struct LuaAnalog<char>{ typedef const char* Result; };
		template<> struct LuaAnalog<const char>{ typedef const char* Result; };
		template<> struct LuaAnalog<int>{ typedef lua_Integer Result; };
		template<> struct LuaAnalog<const int>{ typedef lua_Integer Result; };
		template<> struct LuaAnalog<unsigned int>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<const unsigned int>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<unsigned long>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<const unsigned long>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<double>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<const double>{ typedef lua_Number Result; };
		template<> struct LuaAnalog<char*>{ typedef const char* Result; };
		template<> struct LuaAnalog<const char*>{ typedef const char* Result; };


		template<class T> struct PushAnalog : LuaAnalog<T>{};
		template<> struct PushAnalog<char>{ typedef char Result; };
		template<> struct PushAnalog<const char>{ typedef const char Result; };


		template<class T>
		static bool CompatibleType(lua_State *L, int idx)
		{

			assert(idx > 0);


			if( ConstCheck<T>::bIsConst && CompatibleType<StripConst<T>::Result>(L, idx)) return true;


			PushMetaTableFn pfnPushMetaTable = GetPushMetaTableFn<StripPtr<T>::Result>();
			if( pfnPushMetaTable )
			{
				(*pfnPushMetaTable)(L);
				if( lua_getmetatable(L, idx) )
				{
					bool bCompatibleTypes = true;
					while( lua_equal(L, -1, -2) == 0 )
					{
						lua_getfield(L, -1, "BaseType");
						if( lua_istable(L, -1) ) 
						{
							lua_remove(L, -2);
						}
						else
						{
							lua_pop(L, 3);
							return false;
						}
					}
					

					lua_pop(L, 2);
					return true;
				}
				else
				{
					lua_pop(L, 1);
					return false;
				}
			}
			else if( lua_getmetatable(L, idx) )
			{
				lua_pop(L, 1);
				return false;
			}


			return true;
		}


		static const char *GetTypeNameFromMetatable(lua_State *L, int idx)
		{
			const char *szTypeName = NULL;
			if( lua_getmetatable(L, idx) != 0 )
			{
				lua_getfield(L, -1, "CTypeName");
				szTypeName = lua_tostring(L, -1);
				lua_pop(L, 2);
			}

			if( !szTypeName ) szTypeName = "Unkown_Type";

			return szTypeName;
		}


		template<class T> static const char *GetTypeNameFromMetatable(lua_State *L)
		{
			const char *szTypeName = NULL;
			PushMetaTableFn pfnPushMetaTable = GetPushMetaTableFn<StripPtr<T>::Result>();
			if( pfnPushMetaTable )
			{
				(*pfnPushMetaTable)(L);
				lua_getfield(L, -1, "CTypeName");
				szTypeName = lua_tostring(L, -1);
				lua_pop(L, 2);
			}

			if( !szTypeName ) szTypeName = "Unkown_Type";

			return szTypeName;
		}


		template<class T>
		static inline T Get(lua_State *L, int idx)
		{
			T pkRet = NULL;
			if( lua_istable(L, idx) )
			{

				if( CompatibleType<T>(L, idx) )
				{
					lua_pushvalue(L, idx);
					lua_getfield(L, -1, "CData");
					CData kCData = (CData)lua_touserdata(L, -1);
					pkRet = (T)kCData.GetPtr();
					lua_pop(L, 2);
				}
				else Error(_T("Cannot cast from %s to %s.  Using NULL instead."), GetTypeNameFromMetatable(L, idx), GetTypeNameFromMetatable<T>(L));
			}
			else if( lua_islightuserdata(L, idx) )
			{
				pkRet = (T)lua_touserdata(L, idx);
			}
			else
			{
				Error(_T("Incorrect type for pointer argument.  Using NULL instead."));
			}

			return pkRet;
		}


		template<> static inline bool Get(lua_State *L, int idx) { return lua_toboolean(L, idx) != 0; }
		template<> static inline lua_Integer Get(lua_State *L, int idx) { return lua_tointeger(L, idx); }
#if defined(LEKMOD_MACOS)

#else
		template<> static inline long Get(lua_State *L, int idx) { return (long)lua_tointeger(L, idx); }
#endif
		template<> static inline lua_Number Get(lua_State *L, int idx) { return lua_tonumber(L, idx); }
		template<> static inline float Get(lua_State *L, int idx) { return (float)lua_tonumber(L, idx); }
		template<> static inline const char *Get(lua_State *L, int idx) { return lua_tostring(L, idx); }
		template<> static inline StackValue Get(lua_State *L, int idx) { return StackValue(L, idx); }


		template<class T> static void PushCData(lua_State *L, T *pVal)
		{

			lua_pushlightuserdata(L, pVal);
		}


		template<class T>
		static inline void Push(lua_State *L, T *pVal)
		{

			lua_newtable(L);


			PushCData(L, pVal);
			lua_setfield(L, -2, "CData");


			PushMetaTableFn pfnPushMetaTable = GetPushMetaTableFn<T>();
			if( pfnPushMetaTable )
			{
				(*pfnPushMetaTable)(L);
				lua_setmetatable(L, -2);
			}
		}


		static inline void Push(lua_State *L, const bool bVal) { lua_pushboolean(L, bVal); }
		static inline void Push(lua_State *L, const char cVal) { char szVal[2]; szVal[0] = cVal; szVal[1] = '\0'; lua_pushstring(L, szVal); }
		static inline void Push(lua_State *L, const lua_Integer iVal) { lua_pushinteger(L, iVal); }
		static inline void Push(lua_State *L, const lua_Number fVal) { lua_pushnumber(L, fVal); }
		static inline void Push(lua_State *L, const float fVal) { lua_pushnumber(L, fVal); }
		static inline void Push(lua_State *L, _In_z_ const char *szVal) { lua_pushstring(L, szVal); }
		static inline void Push(lua_State *,  const StackValue kVal) { kVal.Push(); }


		template<class T>
		struct ArgValidator {
			template<class TArgType> static inline void Validate(TArgType&, bool&){}
		};


		template<class T>
		struct ArgValidator<T&>{
			template<class TArgType> static inline void Validate(TArgType &arg, bool &bValid) {}
			static inline void Validate(T *&arg, bool &bValid) {
				if( arg == NULL ) {
					Error(_T("Bad argument: NULL reference"));
					bValid = false;
				}
			}
		};


		template<class T> struct ArgValidator<const T&> : public ArgValidator<T&> {};




		template<>
		struct ArgValidator<unsigned int>{
			static inline void Validate(lua_Number &iArg, bool &bValid) {
				if( iArg < 0 ) {
					Error(_T("Bad argument: Negative number passed for uint"));
					bValid = false;
				}
			}
		};


		template<> struct ArgValidator<const unsigned int> : ArgValidator<unsigned int> {};


		template<>
		struct ArgValidator<char>{
			static inline void Validate(const char *szArg, bool &bValid) {
				size_t uiLength = strlen(szArg);
				if( uiLength == 0 ) {
					Error(_T("Bad argument: Empty string representing character."));
					bValid = false;
				}
				else if( uiLength > 1 ) {
					Error(_T("Bad argument: \"%s\" passed to one character argument."), szArg);
					bValid = false;
				}
			}
		};


		template<> struct ArgValidator<const char> : public ArgValidator<char> {};


		template<class T>
		struct FromLuaAnalog {
			template<class TLuaAnalog> static inline TLuaAnalog &Convert(TLuaAnalog &arg) { return arg; }
		};


		template<class T>
		struct FromLuaAnalog<T&> {
			static inline T &Convert(T *&arg) { FAssertMsg(arg, "Forgot to check for NULL before dereferencing"); return *arg; }
			template<class TArgType> static inline TArgType &Convert(TArgType &arg) { return arg; }
		};


		template<class T> struct FromLuaAnalog<const T&> : public FromLuaAnalog<T&> {};


		template<>
		struct FromLuaAnalog<char> {
			static inline char Convert( _In_z_ const char *szArg)
			{
				assert(szArg && *szArg && "Empty string");
				if( szArg )
					return szArg[0];
				else
					return 0;
			}
		};


		template<> struct FromLuaAnalog<const char> : public FromLuaAnalog<char> {};


		template<>
		struct FromLuaAnalog<unsigned int> {
			static inline unsigned int Convert(const lua_Number fArg) { return (unsigned int)fArg; }
		};


		template<> struct FromLuaAnalog<const unsigned int> : public FromLuaAnalog<unsigned int> {};


		template<class TLuaAnalog>
		struct ToLuaAnalog {
			template<class T> static inline T &Convert(T&arg) { return arg; }
		};


		template<class TLuaAnalog>
		struct ToLuaAnalog<TLuaAnalog*> {
			static inline TLuaAnalog *Convert(TLuaAnalog &arg) { return &arg; }
			static inline const TLuaAnalog *Convert(const TLuaAnalog &arg) { return &arg; }
			template<class T> static inline T &Convert(T&arg) { return arg; }
		};


		template<class TLuaAnalog> struct ToLuaAnalog<const TLuaAnalog*> : public ToLuaAnalog<TLuaAnalog*> {};


		template<>
		struct ToLuaAnalog<const char*> {
			static inline const char *Convert(char &cArg) { return &cArg; }
			template<class T> static inline T &Convert(T&arg) { return arg; }
		};


		template<class TFunc>
		static int Call(TFunc pfn, lua_State *L, const TCHAR *szLuaFnName) {




			FLUA_COMPILE_TIME_ERROR(CantDoCall);
		}


		static inline int Call(lua_CFunction pfn, lua_State *L, const TCHAR *                 ) { return (*pfn)(L); }





		template<class T> struct TypeNameHolder {
			static const char *&Get() { static const char *szName = "Unknown_Type"; return szName; }
		};


		int FunctionToString(lua_State *L);


		void PushCodeBuddyTable(lua_State *L);



		void PushFunctionDocumentationTable(lua_State *L);



		void SetToStringForFunc(lua_State *L, lua_CFunction pfnToString);


		template<class T> static const char *DescribeType() { return TypeNameHolder<StripPtrAndRef<T>::Result>::Get(); }
		template<> static const char *DescribeType<void>() { return "void"; }
		template<> static const char *DescribeType<bool>() { return "bool"; }
		template<> static const char *DescribeType<const bool>() { return "const bool"; }
		template<> static const char *DescribeType<char>() { return "char"; }
		template<> static const char *DescribeType<const char>() { return "const char"; }
		template<> static const char *DescribeType<int>() { return "int"; }
		template<> static const char *DescribeType<const int>() { return "const int"; }
		template<> static const char *DescribeType<unsigned int>() { return "uint"; }
		template<> static const char *DescribeType<const unsigned int>() { return "const uint"; }
		template<> static const char *DescribeType<unsigned long>() { return "unsigned long"; }
		template<> static const char *DescribeType<const unsigned long>() { return "const unsigned long"; }
		template<> static const char *DescribeType<float>() { return "float"; }
		template<> static const char *DescribeType<const float>() { return "const float"; }
		template<> static const char *DescribeType<double>() { return "double"; }
		template<> static const char *DescribeType<const double>() { return "const double"; }
		template<> static const char *DescribeType<char*>() { return "char"; }
		template<> static const char *DescribeType<const char*>() { return "const char"; }
		template<> static const char *DescribeType<StackValue>() { return "variant"; }


		template<class T> struct PtrDescription { static const char *Get() { return ""; } };
		template<class T> struct PtrDescription<T*> { static const char *Get() { return "*"; } };
		template<class T> struct PtrDescription<T&> { static const char *Get() { return "&"; } };



		template<class TFunc>
		static void DescribeFunc(TFunc pfn, _In_z_ const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "CFunction: %s", szFuncName);
		}


		static void DescribeFunc(lua_CFunction, _In_z_ const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "CFunction: %s", szFuncName);
		}



		template<class TSignature>
		void PushFunction(lua_State *L, TSignature pfn, _In_z_ const char *szFuncName)
		{

			union FuncPtrAsLightUserData
			{
				void *pvUserData;
				TSignature pfnFunc;
			};


			struct LuaInterface
			{

				static int Thunk(lua_State *L)
				{
					FuncPtrAsLightUserData kConverter;
					kConverter.pvUserData = lua_touserdata(L, lua_upvalueindex(1));
					const char *szFuncName = lua_tostring(L, lua_upvalueindex(2));
					return Call(kConverter.pfnFunc, L, szFuncName);
				}


				static int ToString(lua_State *L)
				{
					if( lua_iscfunction(L, -1) )
					{

						FuncPtrAsLightUserData kConverter;
						if( lua_getupvalue(L, -1, 1) )
						{
							kConverter.pvUserData = lua_touserdata(L, -1);
							lua_pop(L, 1);
						}
						else
							kConverter.pvUserData = NULL;


						const char *szFuncName;
						if( lua_getupvalue(L, -1, 2) )
						{
							szFuncName = lua_tostring(L, -1);
							lua_pop(L, 1);
						}
						else
							szFuncName = "UnknownFunction";


						char szDescription[256];
						DescribeFunc(kConverter.pfnFunc, szFuncName, szDescription, 256);
						lua_pushstring(L, szDescription);
					}
					else
						lua_pushstring(L, "Misused ToString Function!");

					return 1;
				}
			};


			FuncPtrAsLightUserData kConverter;
			kConverter.pfnFunc = pfn;
			lua_pushlightuserdata(L, kConverter.pvUserData);
			lua_pushstring(L, szFuncName);
			lua_pushcclosure(L, &LuaInterface::Thunk, 2);
			SetToStringForFunc(L, &LuaInterface::ToString);
		}


		bool CCallWithErrorHandling(lua_State *L, lua_CFunction pfn, void *pvUserData = NULL);


		void HandleCPCallError(lua_State *L, int iCPCallResults);
	}





	template<class TFunc>
	static bool MakeProtectedCall(lua_State *L, TFunc pfn) {




		FLUA_COMPILE_TIME_ERROR(CantDoProtectedCall);
		return false;
	}



	template<>
	static bool MakeProtectedCall(lua_State *L, void(*pfn)(lua_State*)) {
		typedef void(*Func)(lua_State*);
		struct CallStruct {
			CallStruct(Func pfnFunc) : m_pfnFunc(pfnFunc) {}
			Func m_pfnFunc;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct != NULL);
				if( pkCallStruct != NULL )
					(*pkCallStruct->m_pfnFunc)(L);

				return 0;
			}
		};

		CallStruct kCall(pfn);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TArg1, class TPassedArg1>
	static bool MakeProtectedCall(lua_State *L, void(*pfn)(lua_State*, TArg1), TPassedArg1 arg1) {
		typedef void(*Func)(lua_State*, TArg1);
		struct CallStruct {
			CallStruct(Func pfnFunc, TArg1 arg1) : m_pfnFunc(pfnFunc), m_Arg1(arg1){}
			Func m_pfnFunc;
			TArg1 m_Arg1;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(*pkCallStruct->m_pfnFunc)(L, pkCallStruct->m_Arg1);
				return 0;
			}
		};

		CallStruct kCall(pfn, arg1);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TArg1, class TArg2, class TPassedArg1, class TPassedArg2>
	static bool MakeProtectedCall(lua_State *L, void(*pfn)(lua_State*, TArg1, TArg2), TPassedArg1 arg1, TPassedArg2 arg2) {
		typedef void(*Func)(lua_State*, TArg1, TArg2);
		struct CallStruct {
			CallStruct(Func pfnFunc, TArg1 arg1, TArg2 arg2) : m_pfnFunc(pfnFunc), m_Arg1(arg1), m_Arg2(arg2) {}
			Func m_pfnFunc;
			TArg1 m_Arg1;
			TArg2 m_Arg2;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(*pkCallStruct->m_pfnFunc)(L, pkCallStruct->m_Arg1, pkCallStruct->m_Arg2);
				return 0;
			}
		};

		CallStruct kCall(pfn, arg1, arg2);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TArg1, class TArg2, class TArg3, class TPassedArg1, class TPassedArg2, class TPassedArg3>
	static bool MakeProtectedCall(lua_State *L, void(*pfn)(lua_State*, TArg1, TArg2, TArg3), TPassedArg1 arg1, TPassedArg2 arg2, TPassedArg3 arg3) {
		typedef void(*Func)(lua_State*, TArg1, TArg2, TArg3);
		struct CallStruct {
			CallStruct(Func pfnFunc, TArg1 arg1, TArg2 arg2, TArg3 arg3) : m_pfnFunc(pfnFunc), m_Arg1(arg1), m_Arg2(arg2), m_Arg3(arg3) {}
			Func m_pfnFunc;
			TArg1 m_Arg1;
			TArg2 m_Arg2;
			TArg3 m_Arg3;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(*pkCallStruct->m_pfnFunc)(L, pkCallStruct->m_Arg1, pkCallStruct->m_Arg2, pkCallStruct->m_Arg3);
				return 0;
			}
		};

		CallStruct kCall(pfn, arg1, arg2, arg3);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TArg1, class TArg2, class TArg3, class TArg4, class TPassedArg1, class TPassedArg2, class TPassedArg3, class TPassedArg4>
	static bool MakeProtectedCall(lua_State *L, void(*pfn)(lua_State*, TArg1, TArg2, TArg3, TArg4),
	                              TPassedArg1 arg1, TPassedArg2 arg2, TPassedArg3 arg3, TPassedArg4 arg4) {
		typedef void(*Func)(lua_State*, TArg1, TArg2, TArg3, TArg4);
		struct CallStruct {
			CallStruct(Func pfnFunc, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4) :
				m_pfnFunc(pfnFunc), m_Arg1(arg1), m_Arg2(arg2), m_Arg3(arg3), m_Arg4(arg4) {}
			Func m_pfnFunc;
			TArg1 m_Arg1;
			TArg2 m_Arg2;
			TArg3 m_Arg3;
			TArg4 m_Arg4;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(*pkCallStruct->m_pfnFunc)(L, pkCallStruct->m_Arg1, pkCallStruct->m_Arg2, pkCallStruct->m_Arg3, pkCallStruct->m_Arg4);
				return 0;
			}
		};

		CallStruct kCall(pfn, arg1, arg2, arg3, arg4);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}



	template<class TClass, class TFuncOwner>
	static bool MakeProtectedCall(lua_State *L, TClass *pkObj, void(TFuncOwner::*pfn)(lua_State*)) {
		typedef void(TFuncOwner::*Func)(lua_State*);
		struct CallStruct {
			CallStruct(TClass *pkObj, Func pfnFunc) : m_pkObj(pkObj), m_pfnFunc(pfnFunc) {}
			TClass *m_pkObj;
			Func m_pfnFunc;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(pkCallStruct->m_pkObj->*(pkCallStruct->m_pfnFunc))(L);
				return 0;
			}
		};

		assert(pkObj);
		CallStruct kCall(pkObj, pfn);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TClass, class TFuncOwner, class TArg1, class TPassedArg1>
	static bool MakeProtectedCall(lua_State *L, TClass *pkObj, void(TFuncOwner::*pfn)(lua_State*, TArg1), TPassedArg1 arg1) {
		typedef void(TFuncOwner::*Func)(lua_State*, TArg1);
		struct CallStruct {
			CallStruct(TClass *pkObj, Func pfnFunc, TArg1 arg1) : m_pkObj(pkObj), m_pfnFunc(pfnFunc), m_Arg1(arg1){}
			TClass *m_pkObj;
			Func m_pfnFunc;
			TArg1 m_Arg1;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(pkCallStruct->m_pkObj->*(pkCallStruct->m_pfnFunc))(L, pkCallStruct->m_Arg1);
				return 0;
			}
		};

		assert(pkObj);
		CallStruct kCall(pkObj, pfn, arg1);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TClass, class TFuncOwner, class TArg1, class TPassedArg1, class TArg2, class TPassedArg2>
	static bool MakeProtectedCall(lua_State *L, TClass *pkObj, void(TFuncOwner::*pfn)(lua_State*, TArg1, TArg2),
	                              TPassedArg1 arg1, TPassedArg2 arg2) {
		typedef void(TFuncOwner::*Func)(lua_State*, TArg1, TArg2);
		struct CallStruct {
			CallStruct(TClass *pkObj, Func pfnFunc, TArg1 arg1, TArg2 arg2) : m_pkObj(pkObj), m_pfnFunc(pfnFunc), m_Arg1(arg1), m_Arg2(arg2) {}
			TClass *m_pkObj;
			Func m_pfnFunc;
			TArg1 m_Arg1;
			TArg2 m_Arg2;


			static int Execute(lua_State *L) {
				CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
				assert(pkCallStruct);
				(pkCallStruct->m_pkObj->*(pkCallStruct->m_pfnFunc))(L, pkCallStruct->m_Arg1, pkCallStruct->m_Arg2);
				return 0;
			}
		};

		assert(pkObj);
		CallStruct kCall(pkObj, pfn, arg1, arg2);
		return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	template<class TClass, class TFuncOwner, class TArg1, class TPassedArg1, class TArg2, class TPassedArg2, class TArg3, class TPassedArg3>
	static bool MakeProtectedCall(lua_State *L, TClass *pkObj, void(TFuncOwner::*pfn)(lua_State*, TArg1, TArg2),
	                              TPassedArg1 arg1, TPassedArg2 arg2, TPassedArg3 arg3) {
			typedef void(TFuncOwner::*Func)(lua_State*, TArg1, TArg2, TArg3);
			struct CallStruct {
				CallStruct(TClass *pkObj, Func pfnFunc, TArg1 arg1, TArg2 arg2, TArg3 arg3) :
					m_pkObj(pkObj), m_pfnFunc(pfnFunc), m_Arg1(arg1), m_Arg2(arg2), m_Arg3(arg3) {}
				TClass *m_pkObj;
				Func m_pfnFunc;
				TArg1 m_Arg1;
				TArg2 m_Arg2;
				TArg3 m_Arg3;


				static int Execute(lua_State *L) {
					CallStruct *pkCallStruct = (CallStruct*)lua_touserdata(L, -1);
					assert(pkCallStruct);
					(pkCallStruct->m_pkObj->*(pkCallStruct->m_pfnFunc))(L, pkCallStruct->m_Arg1, pkCallStruct->m_Arg2, pkCallStruct->m_Arg3);
					return 0;
				}
			};

			assert(pkObj);
			CallStruct kCall(pkObj, pfn, arg1, arg2, arg3);
			return Details::CCallWithErrorHandling(L, &CallStruct::Execute, &kCall);
	}

	extern FCriticalSection	ms_CriticalSection;
}

#if !defined(FINAL_RELEASE)
#define FLUA_OUTPUT_CALLSTACKS
#endif

#ifdef FLUA_OUTPUT_CALLSTACKS
#define FLUA_SAVECALLSTACK(x) FLua::Details::SaveCallStack(x)
#else
#define FLUA_SAVECALLSTACK(x) ((void)0)
#endif

#ifdef FLUA_MULTITHREAD_SUPPORT
 #define FLUA_ENTER_CRITICAL_SECTION		FLua::ms_CriticalSection.Enter()
 #define FLUA_LEAVE_CRITICAL_SECTION		FLua::ms_CriticalSection.Leave()
#else
 #define FLUA_ENTER_CRITICAL_SECTION		((void)0)
 #define FLUA_LEAVE_CRITICAL_SECTION		((void)0)
#endif
#endif
