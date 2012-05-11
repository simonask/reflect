#pragma once
#ifndef REFERENCE_TYPE_HPP_EAHSMBCU
#define REFERENCE_TYPE_HPP_EAHSMBCU

#include "type.hpp"
#include "archive_node.hpp"

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
struct ReferenceTypeImpl : ReferenceType {
	typedef typename T::PointeeType PointeeType;
	
	ReferenceTypeImpl(std::string base_name) : ReferenceType(build_reference_type_name(base_name, get_type<PointeeType>())) {}
	
	// ReferenceType interface
	const Type* pointee_type() const { return get_type<PointeeType>(); }
	
	// Type interface
	size_t size() const { return sizeof(T); }
	void deserialize(byte* place, const ArchiveNode& node) const;
	void serialize(const byte* place, ArchiveNode& node) const;
	void construct(byte* place, IUniverse&) const;
	void destruct(byte* place, IUniverse&) const;
};

template <typename T>
void ReferenceTypeImpl<T>::construct(byte* place, IUniverse&) const {
	new(place) T;
}

template <typename T>
void ReferenceTypeImpl<T>::destruct(byte* place, IUniverse&) const {
	reinterpret_cast<T*>(place)->~T();
}

template <typename T>
void ReferenceTypeImpl<T>::deserialize(byte* place, const ArchiveNode& node) const {
	T& ptr = *reinterpret_cast<T*>(place);
	node.register_reference_for_deserialization(ptr);
}

template <typename T>
void ReferenceTypeImpl<T>::serialize(const byte* place, ArchiveNode& node) const {
	const T& ptr = *reinterpret_cast<const T*>(place);
	node.register_reference_for_serialization(ptr);
}

#endif /* end of include guard: REFERENCE_TYPE_HPP_EAHSMBCU */
