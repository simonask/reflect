#pragma once
#ifndef SIGNAL_HPP_IVWSWZJM
#define SIGNAL_HPP_IVWSWZJM

#include "base/basic.hpp"
#include "type/type.hpp"
#include "object/objectptr.hpp"
#include "serialization/archive_node.hpp"

#include <functional>
#include <sstream>
#include <iostream> // TODO: Get rid of.

struct SlotAttributeBase;
struct Object;
struct ObjectTypeBase;

struct SignalTypeBase;
template <typename... Args> struct SignalType;

struct SlotInvokerBase {
	virtual ~SlotInvokerBase() {}
	virtual Object* receiver() const = 0;
	virtual const SlotAttributeBase* slot() const = 0;
};

template <typename... Args>
struct SlotInvoker : SlotInvokerBase {
	virtual void invoke(Args...) const = 0;
};

template <typename... Args>
class Signal {
public:
	template <typename Receiver, typename R>
	void connect(Receiver* object, R(Receiver::*)(Args...));
	template <typename Receiver, typename R>
	void connect(const Receiver* object, R(Receiver::*)(Args...) const);
	template <typename Receiver, typename R>
	void connect(ObjectPtr<Receiver> object, R(Receiver::*method)(Args...)) { connect(object.get(), method); }
	template <typename Receiver, typename R>
	void connect(ObjectPtr<const Receiver> object, R(Receiver::*method)(Args...) const) { connect(object.get(), method); }
	template <typename R>
	void connect(std::function<R(Args...)>);
	bool connect(ObjectPtr<> receiver, const SlotAttributeBase* slot);
	
	void invoke(Args...) const;
	void operator()(Args... args) const { invoke(std::forward<Args>(args)...); }
	
	size_t num_connections() const { return invokers_.size(); }
	const SlotInvoker<Args...>* connection_at(size_t idx) const { return invokers_[idx]; }
private:
	Array<SlotInvoker<Args...>*> invokers_;
};

struct SignalTypeBase : Type {
public:
	virtual const Array<const Type*>& signature() const = 0;
protected:
	static std::string build_signal_name(const Array<const Type*>& signature);
};

template <typename... Args>
struct SignalType : TypeFor<Signal<Args...>, SignalTypeBase> {
	void deserialize(Signal<Args...>& place, const ArchiveNode&, IUniverse&) const;
	void serialize(const Signal<Args...>& place, ArchiveNode&, IUniverse&) const;
	const std::string& name() const { return name_; }
	size_t size() const { return sizeof(Signal<Args...>); }
	const Array<const Type*>& signature() const { return signature_; }
	
	SignalType() {
		build_signature<Args...>(signature_);
		name_ = SignalTypeBase::build_signal_name(signature_);
	}
private:
	std::string name_;
	Array<const Type*> signature_;
};

template <typename... Args>
struct BuildTypeInfo<Signal<Args...>> {
	static const SignalType<Args...>* build() {
		static const SignalType<Args...> type;
		return &type;
	}
};

template <typename T, typename R, typename... Args>
struct MemberSlotInvoker : SlotInvoker<Args...> {
	typedef R(T::*FunctionType)(Args...);
	T* object_;
	FunctionType member_;
	
	void invoke(Args... args) const {
		(object_->*member_)(std::forward<Args>(args)...);
	}
	
	Object* receiver() const { return object_; }
	
	const SlotAttributeBase* slot() const; 
	
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
		static_cast<SlotInvoker<Args...>*>(invoker)->invoke(std::forward<Args>(args)...);
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

struct SlotAttributeBase {
	SlotAttributeBase(std::string name, std::string description) : name_(name), description_(description) {}
	virtual ~SlotAttributeBase() {}
	const std::string& name() const { return name_; }
	const std::string& description() const { return description_; }
	virtual std::string signature_description() const = 0;
	const Array<const Type*>& signature() const { return signature_; }
private:
	std::string name_;
	std::string description_;
protected:
	Array<const Type*> signature_;
};

template <typename T>
struct SlotForObject {
	virtual ~SlotForObject() {}
	virtual const std::string& slot_name() const = 0;
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
	
	virtual SlotInvoker<Args...>* create_invoker(ObjectPtr<>) const = 0;
};

template <typename... Args>
bool Signal<Args...>::connect(ObjectPtr<> ptr, const SlotAttributeBase* slot) {
	auto slot_with_signature = dynamic_cast<const SlotWithSignature<Args...>*>(slot);
	if (slot_with_signature == nullptr) {
		return false;
	}
	auto invoker = slot_with_signature->create_invoker(ptr);
	if (invoker == nullptr) {
		delete invoker;
		return false;
	}
	invokers_.push_back(invoker);
	return true;
}

template <typename T, typename R, typename... Args>
struct SlotAttribute : SlotForObject<T>, SlotWithSignature<Args...> {
	typedef R(T::*FunctionType)(Args...);
	
	SlotAttribute(std::string name, std::string description, FunctionType function) : SlotWithSignature<Args...>(name, description), function_(function) {}
	
	FunctionType function_;
	
	FunctionType method() const { return function_; }
	
	SlotInvoker<Args...>* create_invoker(ObjectPtr<> base_ptr) const {
		ObjectPtr<T> ptr = aspect_cast<T>(base_ptr);
		if (ptr == nullptr) return nullptr;
		return new MemberSlotInvoker<T, R, Args...>(ptr.get(), function_);
	}
	
	const std::string& slot_name() const { return this->name(); }
};

template <typename... Args>
void SignalType<Args...>::deserialize(Signal<Args...>& signal, const ArchiveNode& node, IUniverse&) const {
	if (node.is_array()) {
		for (size_t i = 0; i < node.array_size(); ++i) {
			const ArchiveNode& connection = node[i];
			if (connection.is_map()) {
				const ArchiveNode& receiver_node = connection["receiver"];
				const ArchiveNode& slot_node = connection["slot"];
				std::string receiver;
				std::string slot;
				if (receiver_node.get(receiver) && slot_node.get(slot)) {
					node.register_signal_for_deserialization(&signal, receiver, slot);
				} else {
					std::cerr << "WARNING: Invalid signal connection.";
				}
			} else {
				std::cerr << "WARNING: Non-map signal connection node. Did you forget to write a scene upgrader?\n";
			}
		}
	}
}

template <typename... Args>
void SignalType<Args...>::serialize(const Signal<Args...>& signal, ArchiveNode& node, IUniverse& universe) const {
	for (size_t i = 0; i < signal.num_connections(); ++i) {
		const SlotInvoker<Args...>* invoker = signal.connection_at(i);
		ObjectPtr<Object> receiver = invoker->receiver();
		const SlotAttributeBase* slot = invoker->slot();
		if (receiver != nullptr && slot != nullptr) {
			ArchiveNode& signal_connection = node.array_push();
			ArchiveNode& receiver_node = signal_connection["receiver"];
			get_type<ObjectPtr<Object>>()->serialize(reinterpret_cast<const byte*>(&receiver), receiver_node, universe); // TODO! Prettier API…!
			signal_connection["slot"] = slot->name();
		}
	}
}

#endif /* end of include guard: SIGNAL_HPP_IVWSWZJM */
