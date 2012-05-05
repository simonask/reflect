#pragma once
#ifndef OBJECT_HPP_P40DARL9
#define OBJECT_HPP_P40DARL9

#include "basic.hpp"

struct Type;
struct DerivedType;
struct StructTypeBase;
template <typename T> struct StructType;

#define REFLECT public: static const bool has_reflection__ = true; typedef StructTypeBase TypeInfoType; static const TypeInfoType* build_type_info__()

template <typename T> const Type* build_type_info(); // Only used for non-reflected types.

struct Object {
	REFLECT;
	
	Object() : type_(nullptr), offset_(0) {}
	virtual ~Object() {}
	
	Object* find_topmost_object();
	const Object* find_topmost_object() const;
	
	const DerivedType* object_type() const { return type_; }
	void set_object_type__(const DerivedType* t) { type_ = t; }
	size_t object_offset() const { return offset_; }
	void set_object_offset__(size_t o) { offset_ = o; }
protected:
	void operator delete(void* ptr) {}
	void* operator new(size_t) { return nullptr; }
private:
	const DerivedType* type_;
	size_t offset_; // offset within composite
};

struct CheckHasBuildTypeInfo {
	template <typename T, const typename T::TypeInfoType*(*)() = T::build_type_info__>
	struct Check {};
};

template <typename T>
struct HasReflection : HasMember<T, CheckHasBuildTypeInfo> {};

template <typename T>
typename std::enable_if<HasReflection<T>::Value, const typename T::TypeInfoType*>::type
get_type() {
	static const typename T::TypeInfoType* p = nullptr;
	if (p == nullptr) {
		p = T::build_type_info__();
	}
	return p;
}

template <typename T>
typename std::enable_if<!HasReflection<T>::Value, const Type*>::type
get_type() {
	static const Type* p = nullptr;
	if (p == nullptr) {
		p = build_type_info<T>();
	}
	return p;
}

inline const DerivedType* get_type(const Object* object) {
	return object->object_type();
}

#endif /* end of include guard: OBJECT_HPP_P40DARL9 */
