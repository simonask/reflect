#pragma once
#ifndef TYPE_HPP_ZXLGBWRF
#define TYPE_HPP_ZXLGBWRF

#include "base/basic.hpp"
#include "base/array.hpp"
#include "object/object.hpp"
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <limits.h>

struct ArchiveNode;
struct IUniverse;
struct SlotAttributeBase;

struct Type {
	virtual void deserialize(byte* place, const ArchiveNode&, IUniverse&) const = 0;
	virtual void serialize(const byte* place, ArchiveNode&, IUniverse&) const = 0;
	virtual void construct(byte* place, IUniverse&) const = 0;
	virtual void destruct(byte* place, IUniverse&) const = 0;
	
	virtual const std::string& name() const = 0;
	virtual size_t size() const = 0;
	virtual bool is_abstract() const { return false; }
protected:
	Type() {}
};

template <typename ObjectType, typename TypeType = Type>
struct TypeFor : TypeType {
	// Forwarding constructor.
	template <typename... Args>
	TypeFor(Args&&... args) : TypeType(std::forward<Args>(args)...) {}
	
	// Override interface.
	virtual void deserialize(ObjectType& place, const ArchiveNode&, IUniverse&) const = 0;
	virtual void serialize(const ObjectType& place, ArchiveNode&, IUniverse&) const = 0;


	// Do not override.
	void deserialize(byte* place, const ArchiveNode& node, IUniverse& universe) const {
		this->deserialize(*reinterpret_cast<ObjectType*>(place), node, universe);
	}
	void serialize(const byte* place, ArchiveNode& node, IUniverse& universe) const {
		this->serialize(*reinterpret_cast<const ObjectType*>(place), node, universe);
	}
	void construct(byte* place, IUniverse&) const {
		::new(place) ObjectType;
	}
	void destruct(byte* place, IUniverse&) const {
		reinterpret_cast<ObjectType*>(place)->~ObjectType();
	}
	size_t size() const { return sizeof(ObjectType); }
};

struct VoidType : Type {
	static const VoidType* get();
	
	void deserialize(byte* place, const ArchiveNode&, IUniverse&) const override {}
	void serialize(const byte*, ArchiveNode&, IUniverse&) const override {}
	virtual void construct(byte*, IUniverse&) const override {}
	virtual void destruct(byte*, IUniverse&) const override {}
	static const std::string Name;
	const std::string& name() const override { return Name; }
	size_t size() const override { return 0; }
	bool is_abstract() const override { return true; }
private:
	VoidType() {}
};

struct SimpleType : Type {
	SimpleType(std::string name, size_t width, size_t component_width, bool is_float, bool is_signed) : name_(std::move(name)), width_(width), component_width_(component_width), is_float_(is_float), is_signed_(is_signed) {}
	const std::string& name() const override { return name_; }
	void construct(byte* place, IUniverse&) const { std::fill(place, place + size(), 0); }
	void destruct(byte*, IUniverse&) const {}
	
	size_t size() const override { return width_; }
	size_t num_components() const { return width_ / component_width_; }
	bool is_signed() const { return is_signed_; }
	virtual void* cast(const SimpleType* to, void* o) const = 0;
protected:
	std::string name_;
	size_t width_;
	size_t component_width_;
	bool is_float_;
	bool is_signed_;
};

struct EnumType : SimpleType {
	EnumType(std::string name, size_t width, bool is_signed = true) : SimpleType(name, width, width, false, is_signed), max_(1LL-SSIZE_MAX), min_(SSIZE_MAX) {}
	void add_entry(std::string name, ssize_t value, std::string description) {
		entries_.emplace_back(std::make_tuple(std::move(name), value, std::move(description)));
	}
	bool contains(ssize_t value) const;
	ssize_t max() const { return max_; }
	ssize_t min() const { return min_; }
	bool name_for_value(ssize_t value, std::string& out_name) const;
	bool value_for_name(const std::string& name, ssize_t& out_value) const;
	
	void deserialize(byte*, const ArchiveNode&, IUniverse&) const override;
	void serialize(const byte*, ArchiveNode&, IUniverse&) const override;
	void* cast(const SimpleType* to, void* o) const;
private:
	Array<std::tuple<std::string, ssize_t, std::string>> entries_;
	ssize_t max_;
	ssize_t min_;
};

struct IntegerType : SimpleType {
	IntegerType(std::string name, size_t width, bool is_signed = true) : SimpleType(name, width, width, false, is_signed) {}
	void deserialize(byte*, const ArchiveNode&, IUniverse&) const override;
	void serialize(const byte*, ArchiveNode&, IUniverse&) const override;
	void* cast(const SimpleType* to, void* o) const;
	size_t max() const;
	ssize_t min() const;
};

struct FloatType : SimpleType {
	FloatType(std::string name, size_t width) : SimpleType(name, width, width, true, true) {}
	void deserialize(byte*, const ArchiveNode& node, IUniverse&) const override;
	void serialize(const byte*, ArchiveNode&, IUniverse&) const override;
	void* cast(const SimpleType* to, void* o) const;
};

struct VectorType : SimpleType {
	VectorType(std::string name, size_t width, size_t component_width, bool is_float, bool is_signed = true) : SimpleType(name, width, component_width, is_float, is_signed) {}
	void deserialize(byte*, const ArchiveNode&, IUniverse&) const override;
	void serialize(const byte*, ArchiveNode&, IUniverse&) const override;
	void* cast(const SimpleType* to, void* o) const;
};

struct StringType : TypeFor<std::string> {
	static const StringType* get();
	
	void deserialize(std::string& place, const ArchiveNode&, IUniverse&) const override;
	void serialize(const std::string& place, ArchiveNode&, IUniverse&) const override;
	
	const std::string& name() const override;
	size_t size() const override { return sizeof(std::string); }
};

struct DerivedType : Type {
	virtual Object* cast(const DerivedType* to, Object* o) const = 0;
	virtual const SlotAttributeBase* get_slot_by_name(const std::string& name) const { return nullptr; }
};

// static_cast
template <typename To, typename From>
typename std::enable_if<std::is_convertible<From*, To*>::value, To*>::type
aspect_cast(From* ptr) {
	return ptr;
}

// complex cast
template <typename To, typename From>
typename std::enable_if<!std::is_convertible<From*, To*>::value, To*>::type
aspect_cast(From* ptr) {
	Object* o = ptr; // check that From derives from Object.
	const Type* from = o->object_type();
	const Type* to = get_type<To>();
	
	auto derived_type = dynamic_cast<const DerivedType*>(from);
	if (derived_type != nullptr) {
		auto other_derived_type = dynamic_cast<const DerivedType*>(to);
		if (other_derived_type != nullptr) {
			return dynamic_cast<To*>(derived_type->cast(other_derived_type, ptr));
		}
		return nullptr;
	}
	
	auto simple_type = dynamic_cast<const SimpleType*>(from);
	if (simple_type != nullptr) {
		auto other_simple_type = dynamic_cast<const SimpleType*>(to);
		if (other_simple_type != nullptr) {
			return static_cast<To*>(simple_type->cast(other_simple_type, ptr));
		}
		return nullptr;
	}
	
	return nullptr;
}

// runtime cast
template <typename From>
Object*
aspect_cast(From* ptr, const DerivedType* to) {
	Object* o = ptr; // check that From derives from Object.
	return o->object_type()->cast(to, o);
}

/*template <typename From>
const Object*
aspect_cast(const From* ptr, const DerivedType* to) {
	const Object* o = ptr;
	return o->object_type()->cast(to, o);
}*/

#define DECLARE_TYPE(T) template<> const Type* build_type_info<T>();
DECLARE_TYPE(int8)
DECLARE_TYPE(int16)
DECLARE_TYPE(int32)
DECLARE_TYPE(int64)
DECLARE_TYPE(uint8)
DECLARE_TYPE(uint16)
DECLARE_TYPE(uint32)
DECLARE_TYPE(uint64)
DECLARE_TYPE(float32)
DECLARE_TYPE(float64)

template <typename T> struct BuildTypeInfo {};

template <> struct BuildTypeInfo<void> {
	static const VoidType* build() { return VoidType::get(); }
};

template <> struct BuildTypeInfo<std::string> {
	static const StringType* build() { return StringType::get(); }
};

template <typename T> const Type* build_type_info() {
	return BuildTypeInfo<T>::build();
}


template <typename Head, typename Next, typename... Rest>
void append_type_names(std::ostream& os) {
	const Type* t = get_type<Head>();
	os << t->name();
	os << ", ";
	append_type_names<Next, Rest...>();
}

template <typename Last = void>
void append_type_names(std::ostream& os) {
	const Type* t = get_type<Last>();
	os << t->name();
}

template <typename... Args>
std::string get_signature_description() {
	std::stringstream ss;
	ss << '(';
	append_type_names<Args...>(ss);
	ss << ')';
	return ss.str();
}


template <typename Head = void>
void build_signature(Array<const Type*>& signature) {
	signature.push_back(get_type<Head>());
}
template <typename Head, typename Next, typename... Rest>
void build_signature(Array<const Type*>& signature) {
	signature.push_back(get_type<Head>());
	build_signature<Next, Rest...>(signature);
}

#endif /* end of include guard: TYPE_HPP_ZXLGBWRF */
