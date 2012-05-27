#pragma once
#ifndef ARRAY_TYPE_HPP_JIO2A6YN
#define ARRAY_TYPE_HPP_JIO2A6YN

#include "type/type.hpp"
#include "serialization/archive_node.hpp"


struct ArrayType : DerivedType {
public:
	const std::string& name() const override { return name_; }
	bool is_variable_length() const { return is_variable_length_; }
	const Type* element_type() const { return element_type_; }
	const Type* type_of_element(size_t idx) const { return element_type(); }
protected:
	ArrayType(std::string name, const Type* element_type, bool is_variable_length) : name_(std::move(name)), element_type_(element_type), is_variable_length_(is_variable_length) {}
	std::string name_;
	const Type* element_type_;
	bool is_variable_length_;
};

struct FixedArrayType : ArrayType {
public:
	FixedArrayType(const Type* element_type, size_t num_elements) : ArrayType(build_fixed_array_type_name(element_type), element_type, false) {}
	size_t num_elements() const { return num_elements_; }
	size_t offset_of_element(size_t idx) const { return idx * element_type_->size(); }
	size_t size() const override { return element_type_->size() * num_elements_; }
	
	void deserialize(byte* place, const ArchiveNode& node, IUniverse&) const override;
	void serialize(const byte* place, ArchiveNode& node, IUniverse&) const override;
protected:
	static std::string build_fixed_array_type_name(const Type* element_type);
	size_t num_elements_;
};

std::string build_variable_length_array_type_name(std::string base_container_name, const Type* element_type);

template <typename Container>
struct VariableLengthArrayType : TypeFor<Container, ArrayType> {
public:
	typedef typename Container::value_type ElementType;
	VariableLengthArrayType(std::string base_container_name) : TypeFor<Container, ArrayType>(build_variable_length_array_type_name(std::move(base_container_name), get_type<ElementType>()), get_type<ElementType>(), true) {}
	size_t num_elements() const { return SIZE_T_MAX; }
	size_t offset_of_element(size_t idx) const { return idx * this->element_type_->size(); }
	
	void deserialize(Container& place, const ArchiveNode& node, IUniverse&) const;
	void serialize(const Container& place, ArchiveNode& node, IUniverse&) const;
	Object* cast(const DerivedType* to, Object* o) const { return nullptr; }
};

template <typename T>
struct BuildTypeInfo<Array<T>> {
	static const ArrayType* build() {
		static const VariableLengthArrayType<Array<T>> type("Array");
		return &type;
	}
};

template <typename T>
void VariableLengthArrayType<T>::deserialize(T& obj, const ArchiveNode& node, IUniverse& universe) const {
	if (node.is_array()) {
		size_t sz = node.array_size();
		obj.reserve(sz);
		for (size_t i = 0; i < sz; ++i) {
			ElementType element;
			get_type<ElementType>()->deserialize(reinterpret_cast<byte*>(&element), node[i], universe);
			obj.push_back(std::move(element));
		}
	}
}

template <typename T>
void VariableLengthArrayType<T>::serialize(const T& obj, ArchiveNode& node, IUniverse& universe) const {
	for (auto& it: obj) {
		ArchiveNode& element = node.array_push();
		get_type<ElementType>()->serialize(reinterpret_cast<const byte*>(&it), element, universe);
	}
}

#endif /* end of include guard: ARRAY_TYPE_HPP_JIO2A6YN */
