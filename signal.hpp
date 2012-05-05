#pragma once
#ifndef SIGNAL_HPP_IVWSWZJM
#define SIGNAL_HPP_IVWSWZJM

#include "basic.hpp"
#include <vector>
#include <functional>
#include <sstream>

template <typename... Args>
struct SlotInvoker {
	virtual void invoke(Args...) const = 0;
};

template <typename... Args>
class Signal {
public:
	template <typename Receiver, typename R>
	void connect(Receiver* object, R(Receiver::*)(Args...));
	template <typename R>
	void connect(std::function<R(Args...)>);
	
	void invoke(Args...) const;
	void operator()(Args... args) const { invoke(std::forward<Args>(args)...); }
private:
	std::vector<SlotInvoker<Args...>*> invokers_;
};

template <typename T, typename R, typename... Args>
struct MemberSlotInvoker : SlotInvoker<Args...> {
	typedef R(T::*FunctionType)(Args...);
	T* object_;
	FunctionType member_;
	
	void invoke(Args... args) const {
		(object_->*member_)(std::forward<Args>(args)...);
	}
	
	MemberSlotInvoker(T* object, FunctionType member) : object_(object), member_(member) {}
};

template <typename R, typename... Args>
struct FunctionInvoker : SlotInvoker<Args...> {
	std::function<R(Args...)> function_;
	
	void invoke(Args... args) const {
		function_(std::forward<Args>(args)...);
	}
	
	FunctionInvoker(std::function<R(Args...)> function) : function_(std::move(function)) {}
};

template <typename... Args>
void Signal<Args...>::invoke(Args... args) const {
	for (auto& invoker: invokers_) {
		invoker->invoke(std::forward<Args>(args)...);
	}
}

template <typename... Args>
template <typename Receiver, typename R>
void Signal<Args...>::connect(Receiver* object, R(Receiver::*member)(Args...)) {
	invokers_.push_back(new MemberSlotInvoker<Receiver, R, Args...>(object, member));
}

template <typename... Args>
template <typename R>
void Signal<Args...>::connect(std::function<R(Args...)> function) {
	invokers_.push_back(new FunctionInvoker<R, Args...>(function));
}

struct SignalAttributeBase {
	SignalAttributeBase(std::string name, std::string description) : name_(std::move(name)), description_(std::move(description)) {}
	virtual ~SignalAttributeBase() {}
	const std::string& name() const { return name_; }
	const std::string& description() const { return description_; }
	virtual std::string signature_description() const = 0;
	const std::vector<const Type*>& signature() const { return signature_; }
private:
	std::string name_;
	std::string description_;
protected:
	std::vector<const Type*> signature_;
};

template <typename T>
struct SignalForObject {
	virtual ~SignalForObject() {}
};

template <typename... Args>
struct SignalWithSignature : SignalAttributeBase {
	SignalWithSignature(std::string name, std::string description) : SignalAttributeBase(name, description) {
		signature_.reserve(sizeof...(Args));
		build_signature<Args...>(signature_);
	}
	virtual ~SignalWithSignature() {}
	
	std::string signature_description() const override {
		return get_signature_description<Args...>();
	}
};

template <typename T, typename... Args>
struct SignalAttribute : SignalForObject<T>, SignalWithSignature<Args...> {
	typedef Signal<Args...> T::* MemberType;
	
	SignalAttribute(std::string name, std::string description, MemberType member) : SignalWithSignature<Args...>(name, description), member_(member) {}
	
	MemberType member_;
};

struct SlotAttributeBase {
	SlotAttributeBase(std::string name, std::string description) : name_(name), description_(description) {}
	virtual ~SlotAttributeBase() {}
	const std::string& name() const { return name_; }
	const std::string& description() const { return description_; }
	virtual std::string signature_description() const = 0;
	const std::vector<const Type*>& signature() const { return signature_; }
private:
	std::string name_;
	std::string description_;
protected:
	std::vector<const Type*> signature_;
};

template <typename T>
struct SlotForObject {
	virtual ~SlotForObject() {}
};

template <typename... Args>
struct SlotWithSignature : SlotAttributeBase {
	SlotWithSignature(std::string name, std::string description) : SlotAttributeBase(name, description) {
		signature_.reserve(sizeof...(Args));
		build_signature<Args...>(signature_);
	}
	virtual ~SlotWithSignature() {}
	
	std::string signature_description() const override {
		return get_signature_description<Args...>();
	}
};

template <typename T, typename R, typename... Args>
struct SlotAttribute : SlotForObject<T>, SlotWithSignature<Args...> {
	typedef R(T::*FunctionType)(Args...);
	
	SlotAttribute(std::string name, std::string description, FunctionType function) : SlotWithSignature<Args...>(name, description), function_(function) {}
	
	FunctionType function_;
};

#endif /* end of include guard: SIGNAL_HPP_IVWSWZJM */
