#pragma once
#ifndef REFLECT_HPP_WJBCX95G
#define REFLECT_HPP_WJBCX95G

#include "object.hpp"
#include "struct_type.hpp"
#include "attribute.hpp"
#include "signal.hpp"

template <typename T>
struct StructTypeBuilder {
	typedef StructTypeBuilder<T> Self;
	
	StructTypeBuilder() : super_(nullptr), is_abstract_(false) {}
	
	Self& abstract(bool a = true) { is_abstract_ = true; return *this; }
	Self& name(std::string n) { name_ = std::move(n); return *this; }
	Self& description(std::string d) { description_ = std::move(d); return *this; }
	Self& super(const StructTypeBase* t) { super_ = t; return *this; }
	
	template <typename MemberType>
	Self& property(MemberType T::* member, std::string name, std::string description/*, MemberType default_value = MemberType()*/) {
		attributes_.push_back(new MemberAttribute<T, MemberType>(std::move(name), std::move(description), member));
		return *this;
	}
	
	template <typename... Args>
	Self& signal(Signal<Args...> T::* member, std::string name, std::string description) {
		signals_.push_back(new SignalAttribute<T, Args...>(std::move(name), std::move(description), member));
		return *this;
	}
	
	template <typename R, typename... Args>
	Self& slot(R(T::*function)(Args...), std::string name, std::string description) {
		slots_.push_back(new SlotAttribute<T, R, Args...>(std::move(name), std::move(description), function));
		return *this;
	}
	
	virtual void define__() = 0;
	
	StructType<T> build__() {
		define__();
		StructType<T> type(super_, std::move(name_), std::move(description_));
		type.set_abstract(is_abstract_);
		type.set_properties(std::move(attributes_));
		return type;
	}
	
	const StructTypeBase* super_;
	bool is_abstract_;
	std::string name_;
	std::string description_;
	std::vector<AttributeForObject<T>*> attributes_;
	std::vector<SignalForObject<T>*> signals_;
	std::vector<SlotForObject<T>*> slots_;
};

#define BEGIN_TYPE_INFO(TYPE) \
const StructTypeBase* TYPE::build_type_info__() { \
	static struct StructTypeBuilderImpl__ : StructTypeBuilder<TYPE> { \
		void define__() override { name(#TYPE);
			
#define END_TYPE_INFO() \
		} \
	} builder__; \
	static const auto t = builder__.build__(); \
	return &t; \
}

#endif /* end of include guard: REFLECT_HPP_WJBCX95G */
