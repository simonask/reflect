#pragma once
#ifndef ATTRIBUTE_HPP_FFQKLYB6
#define ATTRIBUTE_HPP_FFQKLYB6

#include "object.hpp"
#include "type.hpp"
#include "archive.hpp"

struct AttributeBase {
	AttributeBase(std::string name, std::string description) : name_(std::move(name)), description_(std::move(description)) {}
	virtual ~AttributeBase() {}
	
	virtual const Type* type() const = 0;
	const std::string& name() const { return name_; }
	const std::string& description() const { return description_; }
protected:
	std::string name_;
	std::string description_;
};

template <typename T>
struct Attribute : AttributeBase {
	Attribute(std::string name, std::string description) : AttributeBase(std::move(name), std::move(description)) {}
	const Type* type() const override { return get_type<T>(); }
};

template <typename T>
struct AttributeForObject {
	virtual ~AttributeForObject() {}
	virtual const Type* attribute_type() const = 0;
	virtual const std::string& attribute_name() const = 0;
	virtual const std::string& attribute_description() const = 0;
	virtual bool deserialize_attribute(T* object, const ArchiveNode&) const = 0;
	virtual bool serialize_attribute(const T* object, ArchiveNode&) const = 0;
};

template <typename ObjectType, typename MemberType>
struct MemberAttribute : AttributeForObject<ObjectType>, Attribute<MemberType> {
	typedef MemberType ObjectType::* MemberPointer;
	
	MemberAttribute(std::string name, std::string description, MemberPointer member) : Attribute<MemberType>(name, description), member_(member) {}
	
	MemberType& get(ObjectType& object) const {
		return object.*member_;
	}
	
	const MemberType& get(const ObjectType& object) const {
		return object.*member_;
	}
	
	void set(ObjectType& object, MemberType value) const {
		object.*member_ = std::move(value);
	}
	
	const Type* attribute_type() const { return get_type<MemberType>(); }
	const std::string& attribute_name() const { return this->name_; }
	const std::string& attribute_description() const { return this->description_; }
	
	bool deserialize_attribute(ObjectType* object, const ArchiveNode& node) const {
		const Type* t = this->type();
		t->deserialize(reinterpret_cast<byte*>(&(object->*member_)), node);
		return true; // XXX
	}
	
	bool serialize_attribute(const ObjectType* object, ArchiveNode& node) const {
		const Type* t = this->type();
		t->serialize(reinterpret_cast<const byte*>(&(object->*member_)), node);
		return true; // XXX
	}
	
	MemberPointer member_;
};

#endif /* end of include guard: ATTRIBUTE_HPP_FFQKLYB6 */
