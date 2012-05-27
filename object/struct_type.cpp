#include "object/struct_type.hpp"
#include "object/composite_type.hpp"

const ObjectTypeBase* ObjectTypeBase::super() const {
	if (super_ != nullptr) return super_;
	const ObjectTypeBase* object_type = get_type<Object>();
	return object_type != this ? object_type : nullptr;
}

Object* ObjectTypeBase::cast(const DerivedType* to, Object* o) const {
	const ObjectTypeBase* other = dynamic_cast<const ObjectTypeBase*>(to);
	if (other != nullptr) {
		for (const ObjectTypeBase* t = other; t != nullptr; t = t->super_) {
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