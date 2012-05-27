#pragma once
#ifndef OBJECT_HPP_P40DARL9
#define OBJECT_HPP_P40DARL9

#include "base/basic.hpp"

struct IUniverse;
struct Type;
struct DerivedType;
struct ObjectTypeBase;
template <typename T> struct ObjectType;

#define REFLECT \
	public: \
		static const bool has_reflection__ = true; \
		typedef ObjectTypeBase TypeInfoType; \
		static const TypeInfoType* build_type_info__();

template <typename T> const Type* build_type_info(); // Only used for non-reflected types.

struct Object {
	REFLECT;
	
	Object() : type_(nullptr), offset_(0), universe_(nullptr) {}
	virtual ~Object() {}
	
	Object* find_parent();
	const Object* find_parent() const;
	Object* find_topmost_object();
	const Object* find_topmost_object() const;
	
	IUniverse* universe() const { return universe_; }
	void set_universe__(IUniverse* universe) { universe_ = universe; }
	
	const std::string& object_id() const;
	bool set_object_id(std::string new_id);
	
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
	IUniverse* universe_;
};

template <typename T>
struct IsDerivedFromObject {
	static const bool Value = std::is_convertible<typename std::remove_const<T>::type*, Object*>::value;
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
	return T::build_type_info__();
}

template <typename T>
typename std::enable_if<!HasReflection<T>::Value, const Type*>::type
get_type() {
 	return build_type_info<T>();
}

template <typename T>
typename std::enable_if<HasReflection<T>::Value, const DerivedType*>::type
get_type(const T& object) {
	return object.object_type();
}

template <typename T>
typename std::enable_if<!HasReflection<T>::Value, const Type*>::type
get_type(const T& value) {
	return get_type<T>();
}

inline const DerivedType* get_type(Object* object) {
	return object->object_type();
}

inline const DerivedType* get_type(const Object* object) {
	return object->object_type();
}

#endif /* end of include guard: OBJECT_HPP_P40DARL9 */
