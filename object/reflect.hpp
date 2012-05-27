#pragma once
#ifndef REFLECT_HPP_WJBCX95G
#define REFLECT_HPP_WJBCX95G

#include "object/object.hpp"
#include "object/struct_type.hpp"
#include "type/attribute.hpp"
#include "object/signal.hpp"
#include <algorithm>

template <typename T>
struct ObjectTypeBuilder {
	typedef ObjectTypeBuilder<T> Self;
	
	ObjectTypeBuilder() : super_(nullptr), is_abstract_(false) {}
	
	Self& abstract(bool a = true) { is_abstract_ = true; return *this; }
	Self& name(std::string n) { name_ = std::move(n); return *this; }
	Self& description(std::string d) { description_ = std::move(d); return *this; }
	Self& super(const ObjectTypeBase* t) { super_ = t; return *this; }
	
	void check_attribute_name_(const std::string& name) {
		const auto reserved_names = {"class", "aspects"};
		for (auto it: reserved_names) {
			if (name == it) {
				fprintf(stderr, "The attribute name '%s' is reserved.\n", name.c_str());
				ASSERT(false);
			}
		}
	}
	
	template <typename MemberType>
	Self& property(MemberType T::* member, std::string name, std::string description/*, MemberType default_value = MemberType()*/) {
		check_attribute_name_(name);
		attributes_.push_back(new MemberAttribute<T, MemberType>(std::move(name), std::move(description), member));
		return *this;
	}
	
	template <typename GetterReturnType, typename SetterArgumentType, typename SetterReturnType>
	Self& property(GetterReturnType (T::*getter)() const, SetterReturnType (T::*setter)(SetterArgumentType), std::string name, std::string description) {
		check_attribute_name_(name);
		typedef typename RemoveConstRef<GetterReturnType>::Type RawType;
		attributes_.push_back(new MethodAttribute<T, RawType, GetterReturnType, SetterArgumentType, SetterReturnType>(std::move(name), std::move(description), getter, setter));
		return *this;
	}
	
	template <typename... Args>
	Self& signal(Signal<Args...> T::* member, std::string name, std::string description) {
		return property(member, name, description);
	}
	
	template <typename R, typename... Args>
	Self& slot(R(T::*function)(Args...), std::string name, std::string description) {
		slots_.push_back(new SlotAttribute<T, R, Args...>(std::move(name), std::move(description), function));
		return *this;
	}
	
	virtual void define__() = 0;
	
	ObjectType<T> build__() {
		define__();
		ObjectType<T> type(super_, std::move(name_), std::move(description_));
		type.set_abstract(is_abstract_);
		type.set_properties(std::move(attributes_));
		type.set_slots(std::move(slots_));
		return type;
	}
	
	const ObjectTypeBase* super_;
	bool is_abstract_;
	std::string name_;
	std::string description_;
	Array<AttributeForObject<T>*> attributes_;
	Array<SlotForObject<T>*> slots_;
};

#define BEGIN_TYPE_INFO(TYPE) \
const ObjectTypeBase* TYPE::build_type_info__() { \
	static struct ObjectTypeBuilderImpl__ : ObjectTypeBuilder<TYPE> { \
		void define__() override { name(#TYPE);
			
#define END_TYPE_INFO() \
		} \
	} builder__; \
	static const auto t = builder__.build__(); \
	return &t; \
}

#endif /* end of include guard: REFLECT_HPP_WJBCX95G */
