#pragma once
template <class T>
class Delegate;

template <typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
public:
	using MemberCallback = ReturnType(__thiscall*)(void*, Args...);
	using StaticCallback = ReturnType(*)(Args...);

	Delegate() :
		memberFunctionCallback(nullptr),
		staticFunctionCallback(nullptr),
		instance(nullptr)
	{
	}

	Delegate(MemberCallback memberFunctionCallback, StaticCallback staticFunctionCallback, void* instance) :
		memberFunctionCallback(memberFunctionCallback),
		staticFunctionCallback(staticFunctionCallback),
		instance(instance)
	{
	}

	ReturnType operator()(Args... args)
	{
		if (memberFunctionCallback)
		{
			return reinterpret_cast<ReturnType(__thiscall*)(void*, Args...)>(memberFunctionCallback)(instance, args...);
		}

		return staticFunctionCallback(args...);
	}

private:
	MemberCallback memberFunctionCallback;
	StaticCallback staticFunctionCallback;
	void* instance;
};

template <typename InstanceType, typename T>
class MemberDelegate;

template <typename InstanceType, typename ReturnType, typename... Args>
class MemberDelegate<InstanceType, ReturnType(Args...)> : public Delegate<ReturnType(Args...)>
{
private:
	using MemberFunction = ReturnType(InstanceType::*)(Args...);

	union MemberFunctionCaster
	{
		MemberFunction memberFunction;
		typename Delegate<ReturnType(Args...)>::MemberCallback memberFunctionCallback;
	};

public:
	MemberDelegate(InstanceType* instance, MemberFunction memberFunction) :
		Delegate<ReturnType(Args...)>(GetMemberCb(memberFunction), nullptr, instance)
	{
	}

private:
	typename Delegate<ReturnType(Args...)>::MemberCallback GetMemberCb(MemberFunction memberFunction)
	{
		MemberFunctionCaster memberFunctionCaster;

		memberFunctionCaster.memberFunction = memberFunction;

		return memberFunctionCaster.memberFunctionCallback;
	}
};

template <class T>
class StaticDelegate;

template <typename ReturnType, typename... Args>
class StaticDelegate<ReturnType(Args...)> : public Delegate<ReturnType(Args...)>
{
public:
	StaticDelegate(typename Delegate<ReturnType(Args...)>::StaticCallback staticFunctionCallback) :
		Delegate<ReturnType(Args...)>(nullptr, staticFunctionCallback, nullptr)
	{
	}
};
