#include "struct_type.hpp"
#include "composite_type.hpp"

const StructTypeBase* StructTypeBase::super() const {
	if (super_ != nullptr) return super_;
	const StructTypeBase* object_type = get_type<Object>();
	return object_type != this ? object_type : nullptr;
}

Object* StructTypeBase::cast(const DerivedType* to, Object* o) const {
	const StructTypeBase* other = dynamic_cast<const StructTypeBase*>(to);
	if (other != nullptr) {
		for (const StructTypeBase* t = other; t != nullptr; t = t->super_) {
			if (t == this) return o; // TODO: Consider what could be done for multiple inheritance?
		}
		return nullptr;
	}
	
	const CompositeType* comp = dynamic_cast<const CompositeType*>(to);
	if (comp != nullptr) {
		return comp->find_self_up(o);
	}
	
	return nullptr;
}