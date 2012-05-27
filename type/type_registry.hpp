#pragma once
#ifndef TYPE_REGISTRY_HPP_LPQGF8DT
#define TYPE_REGISTRY_HPP_LPQGF8DT

#include "object/object.hpp"
#include <string>

class TypeRegistry {
public:
	template <typename T>
	static void add();
	static void add(const ObjectTypeBase* type);
	
	static const ObjectTypeBase* get(const std::string& name);
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
