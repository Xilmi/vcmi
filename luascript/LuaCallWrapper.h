/*
 * LuaCallWrapper.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "api/Registry.h"
#include "LuaStack.h"
#include <type_traits>

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting
{

// trait to decompose a member function pointer (non-const and const)
template<typename M> struct LuaClassMemberTraits;

// non-const member function
template<typename R, typename C, typename... Args>
struct LuaClassMemberTraits<R(C::*)(Args...)>
{
	using ReturnType = R;
	using ClassType  = C;
	using TupleType  = std::tuple<std::remove_cvref_t<Args>...>;
	static constexpr bool isConst = false;
};

// const member function
template<typename R, typename C, typename... Args>
struct LuaClassMemberTraits<R(C::*)(Args...) const>
{
	using ReturnType = R;
	using ClassType  = C;
	using TupleType  = std::tuple<std::remove_cvref_t<Args>...>;
	static constexpr bool isConst = true;
};

// const noexcept member function
template<typename R, typename C, typename... Args>
struct LuaClassMemberTraits<R(C::*)(Args...) const noexcept>
{
	using ReturnType = R;
	using ClassType  = C;
	using TupleType  = std::tuple<std::remove_cvref_t<Args>...>;
	static constexpr bool isConst = true;
};

/// Adapts a C++ member function (from a proxy class) into a Lua C function with full exception-to-error translation.
/// Automatically unpacks `self` and all arguments from the Lua stack using LuaStack type traits.
/// Usage: LuaMethodWrapper<&Class::method> when Class has a scripting tag,
///        LuaMethodWrapper<&Base::method, DerivedClass> when the method is defined in an untagged base class.
template <auto method, typename ExplicitObjectType = void>
class LuaMethodWrapper
{
	using MethodType = decltype(method);
	using TraitsInfo = LuaClassMemberTraits<MethodType>;
	using ReturnType = typename TraitsInfo::ReturnType;
	using TupleType = typename TraitsInfo::TupleType;
	using ObjectType = std::conditional_t<std::is_void_v<ExplicitObjectType>, typename TraitsInfo::ClassType, ExplicitObjectType>;

	static constexpr bool isSharedPtr = std::is_base_of_v<scripting::TagSharedPointer, ObjectType>;
	static constexpr bool isRawPtr = std::is_base_of_v<scripting::TagRawPointer, ObjectType>;
	static constexpr bool isCopyable = std::is_base_of_v<scripting::TagCopyable, ObjectType>;
	static_assert(isSharedPtr + isRawPtr + isCopyable == 1, "Unsupported class passed into LuaMethodWrapper. Please inherit from scripting API tags");

	using ObjectRawPtr = std::conditional_t<TraitsInfo::isConst, const ObjectType*, ObjectType*>;
	using ObjectSharedPtr = std::conditional_t<TraitsInfo::isConst, std::shared_ptr<const ObjectType>, std::shared_ptr<ObjectType>>;
	using ObjectPtr = std::conditional_t<isSharedPtr, ObjectSharedPtr, std::conditional_t<isCopyable, ObjectType, ObjectRawPtr>>;

	template <std::size_t... I>
	static void tryGetAllImpl(LuaStack &stack, TupleType &t, std::index_sequence<I...>)
	{
		( (stack.get(static_cast<int>(I + 2), std::get<I>(t))), ... );
	}

	template <typename... Ts>
	static void tryGetAll(LuaStack &stack, std::tuple<Ts...> &t)
	{
		tryGetAllImpl(stack, t, std::index_sequence_for<Ts...>{});
	}

	static int invokeImpl(lua_State * L)
	{
		LuaStack S(L);
		ObjectPtr obj{};
		TupleType args;

		S.get(1, obj);
		tryGetAll(S, args);
		S.clear();

		ObjectRawPtr objPtr = nullptr;

		if constexpr (isSharedPtr)
			objPtr = obj.get();
		else if constexpr (isCopyable)
			objPtr = &obj;
		else
			objPtr = obj;

		if constexpr (std::is_void_v<ReturnType>)
		{
			std::apply([&](auto &&... a){ std::invoke(method, objPtr, std::forward<decltype(a)>(a)...); }, args);
			return 0;
		}
		else
		{
			ReturnType result = std::apply([&](auto &&... a){ return std::invoke(method, objPtr, std::forward<decltype(a)>(a)...); }, args);
			S.push(std::move(result));
			return S.stackSize();
		}
	}

public:
	static int invoke(lua_State * L)
	{
		try {
			return invokeImpl(L);
		}
		catch (const std::exception & e)
		{
			lua_pushstring(L, e.what());
		}
		// WARNING: lua_error never returns and performs long jump
		// this method must not have any local variables with non-trivial destructor to avoid leaks
		return lua_error(L);
	}
};

/// trait to decompose a free function pointer
template<typename F> struct LuaFunctionTraits;

template<typename R, typename... Args>
struct LuaFunctionTraits<R(*)(Args...)>
{
	using ReturnType = R;
	using TupleType  = std::tuple<std::remove_cvref_t<Args>...>;
};

// function reference (e.g., R(&)(Args...))
template<typename R, typename... Args>
struct LuaFunctionTraits<R(&)(Args...)> : LuaFunctionTraits<R(*)(Args...)> {};

// function type (R(Args...))
template<typename R, typename... Args>
struct LuaFunctionTraits<R(Args...)> : LuaFunctionTraits<R(*)(Args...)> {};

/// Adapts a C++ free function into a Lua C function, unpacking all arguments from the Lua stack.
/// Use for static proxy methods that need adapted signatures (e.g. fixed extra parameters).
template <auto func>
class LuaFunctionWrapper
{
	using FuncType = decltype(func);
	using TraitsInfo = LuaFunctionTraits<std::remove_pointer_t<std::decay_t<FuncType>>>;
	using ReturnType = typename TraitsInfo::ReturnType;
	using TupleType = typename TraitsInfo::TupleType;

	template <std::size_t... I>
	static void tryGetAllImpl(LuaStack &stack, TupleType &t, std::index_sequence<I...>)
	{
		( (stack.get(static_cast<int>(I + 1), std::get<I>(t))), ... ); // args start at index 1 for free functions
	}

	template <typename... Ts>
	static void tryGetAll(LuaStack &stack, std::tuple<Ts...> &t)
	{
		tryGetAllImpl(stack, t, std::index_sequence_for<Ts...>{});
	}

	static int invokeImpl(lua_State * L)
	{
		LuaStack S(L);
		TupleType args;

		tryGetAll(S, args);
		S.clear();

		if constexpr (std::is_void_v<ReturnType>)
		{
			std::apply([&](auto &&... a){ std::invoke(func, std::forward<decltype(a)>(a)...); }, args);
			return 0;
		}
		else
		{
			ReturnType result = std::apply([&](auto &&... a){ return std::invoke(func, std::forward<decltype(a)>(a)...); }, args);
			S.push(std::move(result));
			return S.stackSize();
		}
	}

public:
	static int invoke(lua_State * L)
	{
		try {
			return invokeImpl(L);
		}
		catch (const std::exception & e)
		{
			lua_pushstring(L, e.what());
		}
		// WARNING: lua_error never returns and performs long jump
		// this method must not have any local variables with non-trivial destructor to avoid leaks
		return lua_error(L);
	}
};

/// Thin wrapper for raw `int(lua_State*)` functions that translates C++ exceptions into Lua errors.
/// Use when a method already has the correct Lua C function signature but needs safe exception handling.
template <auto func>
class LuaCallWrapper
{
public:
	static int invoke(lua_State * L)
	{
		try {
			return func(L);
		}
		catch (const std::exception & e)
		{
			lua_pushstring(L, e.what());
		}
		// WARNING: lua_error never returns and performs long jump
		// this method must not have any local variables with non-trivial destructor to avoid leaks
		return lua_error(L);
	}
};


}

VCMI_LIB_NAMESPACE_END
