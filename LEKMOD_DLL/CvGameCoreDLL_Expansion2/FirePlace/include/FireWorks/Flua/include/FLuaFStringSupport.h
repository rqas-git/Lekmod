











#ifndef FLuaFStringSupport_h
#define FLuaFStringSupport_h
#pragma once

namespace FLua
{
	namespace Details
	{
		template<> struct LuaAnalog<FStringA>{ typedef const char* Result; };
		template<> struct LuaAnalog<FStringA&>{ typedef FStringA Result; };
		template<> struct LuaAnalog<const FStringA&>{ typedef const char *Result; };

		template<> static inline FStringA Get(lua_State *L, int idx) { return lua_tostring(L, idx); }
		static inline void Push(lua_State *L, const FStringA &sVal) { lua_pushstring(L, sVal.c_str()); }

		template<> static const char *DescribeType<FStringA>() { return "FString"; }
		template<> static const char *DescribeType<FStringA&>() { return "FString"; }
		template<> static const char *DescribeType<const FStringA&>() { return "const FString"; }


		static inline void Push(lua_State *L, const wchar* szVal)
		{

			FStringA sConverted;
			FStringW::CopyToUTF8(szVal, &sConverted);
			lua_pushstring(L, sConverted.c_str());
		}

		template<> static const char *DescribeType<const wchar*>() { return "const wchar"; }

		template<> struct LuaAnalog<FStringW&>{ typedef FStringW Result; };
		template<> struct LuaAnalog<const FStringW&>{ typedef FStringW Result; };

		template<> static inline FStringW Get(lua_State *L, int idx)
		{ 
			FStringW sConvert;
			sConvert.CopyUTF8(lua_tostring(L, idx));
			return sConvert; 
		}

		static inline void Push(lua_State *L, const FStringW &sVal)
		{
			Push(L, sVal.c_str());
		}

		template<> static const char *DescribeType<FStringW>() { return "FString"; }
		template<> static const char *DescribeType<FStringW&>() { return "FString"; }
		template<> static const char *DescribeType<const FStringW&>() { return "const FString"; }
	}
}

#endif
