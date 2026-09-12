
#pragma once 

#include "CvLuaArgTemplates.h"

template< class Derived, class InstanceType>
class CvLuaMethodWrapper
{
#if defined(LEKMOD_MACOS)

	friend Derived;
#endif

protected:




	template<typename ret>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)() const);
	template<typename ret, typename arg1>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1) const);
	template<typename ret, typename arg1, typename arg2>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2) const);
	template<typename ret, typename arg1, typename arg2, typename arg3>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3) const);
	template<typename ret, typename arg1, typename arg2, typename arg3, typename arg4>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3, arg4) const);


	template<typename ret>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)());
	template<typename ret, typename arg1>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1));
	template<typename ret, typename arg1, typename arg2>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2));
	template<typename ret, typename arg1, typename arg2, typename arg3>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3));
	template<typename ret, typename arg1, typename arg2, typename arg3, typename arg4>
	static int BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3, arg4));


	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)() const);
	template<typename arg1>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1) const);
	template<typename arg1, typename arg2>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2) const);
	template<typename arg1, typename arg2, typename arg3>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3) const);
	template<typename arg1, typename arg2, typename arg3, typename arg4>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3, arg4) const);


	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)());
	template<typename arg1>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1));
	template<typename arg1, typename arg2>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2));
	template<typename arg1, typename arg2, typename arg3>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3));
	template<typename arg1, typename arg2, typename arg3, typename arg4>
	static int BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3, arg4));
};







template<class Derived, class InstanceType> template<typename ret>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)() const)
{
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)());

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2, typename arg3>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2, typename arg3, typename arg4>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3, arg4) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2), CvLuaArgs::toValue<arg4>(L, idx + 3)));

	return 1;
}




template<class Derived, class InstanceType> template<typename ret>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)())
{
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)());

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2, typename arg3>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2)));

	return 1;
}

template<class Derived, class InstanceType> template<typename ret, typename arg1, typename arg2, typename arg3, typename arg4>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, ret (InstanceType::*func)(arg1, arg2, arg3, arg4))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	CvLuaArgs::pushValue<ret>(L, (pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2), CvLuaArgs::toValue<arg4>(L, idx + 3)));

	return 1;
}




template<class Derived, class InstanceType>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)() const)
{
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)();
	return 0;
}

template<class Derived, class InstanceType> template<typename arg1>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx));
	return 0;
}

template<class Derived, class InstanceType> template<typename arg1, typename arg2>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1));
	return 0;
}


template<class Derived, class InstanceType> template<typename arg1, typename arg2, typename arg3>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2));
	return 0;
}


template<class Derived, class InstanceType> template<typename arg1, typename arg2, typename arg3, typename arg4>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3, arg4) const)
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2), CvLuaArgs::toValue<arg4>(L, idx + 3));
	return 0;
}




template<class Derived, class InstanceType>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)())
{
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)();
	return 0;
}

template<class Derived, class InstanceType> template<typename arg1>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx));
	return 0;
}

template<class Derived, class InstanceType> template<typename arg1, typename arg2>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1));
	return 0;
}


template<class Derived, class InstanceType> template<typename arg1, typename arg2, typename arg3>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2));
	return 0;
}


template<class Derived, class InstanceType> template<typename arg1, typename arg2, typename arg3, typename arg4>
int CvLuaMethodWrapper<Derived, InstanceType>::BasicLuaMethod(lua_State* L, void (InstanceType::*func)(arg1, arg2, arg3, arg4))
{
	const int idx = Derived::GetStartingArgIndex();
	InstanceType* pkType = Derived::GetInstance(L);
	(pkType->*func)(CvLuaArgs::toValue<arg1>(L, idx), CvLuaArgs::toValue<arg2>(L, idx + 1), CvLuaArgs::toValue<arg3>(L, idx + 2), CvLuaArgs::toValue<arg4>(L, idx + 3));
	return 0;
}
