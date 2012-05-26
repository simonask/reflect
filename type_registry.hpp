#pragma once
#ifndef TYPE_REGISTRY_HPP_LPQGF8DT
#define TYPE_REGISTRY_HPP_LPQGF8DT

#include "object.hpp"
#include <string>

class TypeRegistry {
public:
	template <typename T>
	static void add();
	static void add(const StructTypeBase* type);
	
	static const StructTypeBase* get(const std::string& name);
private:
	TypeRegistry();
	struct Impl;
	static Impl* impl();
};

template <typename T>
void TypeRegistry::add() {
	add(get_type<T>());
}


#endif /* end of include guard: TYPE_REGISTRY_HPP_LPQGF8DT */
