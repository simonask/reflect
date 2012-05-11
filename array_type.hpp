#pragma once
#ifndef ARRAY_TYPE_HPP_JIO2A6YN
#define ARRAY_TYPE_HPP_JIO2A6YN

#include "type.hpp"
#include "archive_node.hpp"


struct ArrayType : DerivedType {
public:
	const std::string& name() const override { return name_; }
	bool is_variable_length() const { return is_variable_length_; }
	const Type* element_type() const { return element_type_; }
	const Type* type_of_element(size_t idx) const override { return element_type(); }
protected:
	ArrayType(std::string name, const Type* element_type, bool is_variable_length) : name_(std::move(name)), element_type_(element_type), is_variable_length_(is_variable_length) {}
	std::string name_;
	const Type* element_type_;
	bool is_variable_length_;
};

struct FixedArrayType : ArrayType {
public:
	FixedArrayType(const Type* element_type, size_t num_elements) : ArrayType(build_fixed_array_type_name(element_type), element_type, false) {}
	size_t num_elements() const override { return num_elements_; }
	size_t offset_of_element(size_t idx) const override { return idx * element_type_->size(); }
	size_t size() const override { return element_type_->size() * num_elements_; }
	
	void deserialize(byte* place, const ArchiveNode& node) const override;
	void serialize(const byte* place, ArchiveNode& node) const override;
protected:
	static std::string build_fixed_array_type_name(const Type* element_type);
	size_t num_elements_;
};

std::string build_variable_length_array_type_name(std::string base_container_name, const Type* element_type);

template <typename Container>
struct VariableLengthArrayType : ArrayType {
public:
	typedef typename Container::value_type ElementType;
	VariableLengthArrayType(std::string base_container_name) : ArrayType(build_variable_length_array_type_name(std::move(base_container_name), get_type<ElementType>()), get_type<ElementType>(), true) {}
	size_t num_elements() const override { return SIZE_T_MAX; }
	size_t offset_of_element(size_t idx) const override { return idx * element_type_->size(); }
	size_t size() const override { return sizeof(Container); }
	
	void construct(byte* place, IUniverse&) const override;
	void destruct(byte* place, IUniverse&) const override;
	void deserialize(byte* place, const ArchiveNode& node) const override;
	void serialize(const byte* place, ArchiveNode& node) const override;
	Object* cast(const DerivedType* to, Object* o) const override { return nullptr; }
};

template <typename T>
struct BuildTypeInfo<Array<T>> {
	static const ArrayType* build() {
		static const VariableLengthArrayType<Array<T>> type("Array");
		return &type;
	}
};

template <typename T>
void VariableLengthArrayType<T>::construct(byte* place, IUniverse&) const {
	new(place) T;
}

template <typename T>
void VariableLengthArrayType<T>::destruct(byte* place, IUniverse&) const {
	reinterpret_cast<T*>(place)->~T();
}

template <typename T>
void VariableLengthArrayType<T>::deserialize(byte* place, const ArchiveNode& node) const {
	T* obj = reinterpret_cast<T*>(place);
	if (node.is_array()) {
		size_t sz = node.array_size();
		obj->reserve(sz);
		for (size_t i = 0; i < sz; ++i) {
			ElementType element;
			get_type<ElementType>()->deserialize(reinterpret_cast<byte*>(&element), node[i]);
			obj->push_back(std::move(element));
		}
	}
}

template <typename T>
void VariableLengthArrayType<T>::serialize(const byte* place, ArchiveNode& node) const {
	const T* obj = reinterpret_cast<const T*>(place);
	for (auto& it: *obj) {
		ArchiveNode& element = node.array_push();
		get_type<ElementType>()->serialize(reinterpret_cast<const byte*>(&it), element);
	}
}

#endif /* end of include guard: ARRAY_TYPE_HPP_JIO2A6YN */
