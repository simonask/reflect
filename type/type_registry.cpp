#include "type/type_registry.hpp"
#include "object/struct_type.hpp"
#include "base/basic.hpp"
#include <map>

struct TypeRegistry::Impl {
	std::map<std::string, const ObjectTypeBase*> type_map;
};

TypeRegistry::Impl* TypeRegistry::impl() {
	static Impl* i = new Impl;
	return i;
}

void TypeRegistry::add(const ObjectTypeBase* type) {
	impl()->type_map[type->name()] = type;
}

const ObjectTypeBase* TypeRegistry::get(const std::string& name) {
	return find_or(impl()->type_map, name, nullptr);
}