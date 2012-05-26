#include "type_registry.hpp"
#include "struct_type.hpp"
#include "basic.hpp"
#include <map>

struct TypeRegistry::Impl {
	std::map<std::string, const StructTypeBase*> type_map;
};

TypeRegistry::Impl* TypeRegistry::impl() {
	static Impl* i = new Impl;
	return i;
}

void TypeRegistry::add(const StructTypeBase* type) {
	impl()->type_map[type->name()] = type;
}

const StructTypeBase* TypeRegistry::get(const std::string& name) {
	return find_or(impl()->type_map, name, nullptr);
}