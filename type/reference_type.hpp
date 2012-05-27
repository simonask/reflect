#pragma once
#ifndef REFERENCE_TYPE_HPP_EAHSMBCU
#define REFERENCE_TYPE_HPP_EAHSMBCU

#include "type/type.hpp"
#include "serialization/archive_node.hpp"

struct ReferenceType : Type {
	ReferenceType(std::string name) : name_(std::move(name)) {}
	
	virtual const Type* pointee_type() const = 0;
	
	const std::string& name() const override { return name_; }
protected:
	static std::string build_reference_type_name(std::string base_name, const Type* pointee);
private:
	std::string name_;
};

template <typename T>
struct ReferenceTypeImpl : TypeFor<T, ReferenceType> {
	typedef typename T::PointeeType PointeeType;
	
	ReferenceTypeImpl(std::string base_name) : TypeFor<T, ReferenceType>(ReferenceType::build_reference_type_name(base_name, get_type<PointeeType>())) {}
	
	// ReferenceType interface
	const Type* pointee_type() const { return get_type<PointeeType>(); }
	
	// Type interface
	void deserialize(T& ptr, const ArchiveNode& node, IUniverse&) const;
	void serialize(const T& ptr, ArchiveNode& node, IUniverse&) const;
};

template <typename T>
void ReferenceTypeImpl<T>::deserialize(T& ptr, const ArchiveNode& node, IUniverse&) const {
	node.register_reference_for_deserialization(ptr);
}

template <typename T>
void ReferenceTypeImpl<T>::serialize(const T& ptr, ArchiveNode& node, IUniverse&) const {
	node.register_reference_for_serialization(ptr);
}

#endif /* end of include guard: REFERENCE_TYPE_HPP_EAHSMBCU */
