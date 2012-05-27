#include "object/composite_type.hpp"
#include "object/struct_type.hpp"

CompositeType::CompositeType(std::string name, const ObjectTypeBase* base_type) : base_type_(base_type), name_(std::move(name)), frozen_(false) {
	size_ = this->base_type()->size();
}

void CompositeType::add_aspect(const DerivedType* aspect) {
	ASSERT(!frozen_);
	ASSERT(!aspect->is_abstract());
	aspects_.push_back(aspect); // TODO: Check for circular dependencies.
	size_ += aspect->size();
}

const ObjectTypeBase* CompositeType::base_type() const {
	return base_type_ ? base_type_ : get_type<Object>();
}

Object* CompositeType::cast(const DerivedType* to, Object* o) const {
	return cast(to, o, nullptr);
}

Object* CompositeType::cast(const DerivedType* to, Object* o, const DerivedType* avoid) const {
	if (to == this) return o;
	
	// Check base type and aspects
	Object* result = find_instance_down(to, o, avoid);
	if (result != nullptr) return result;
	
	// Then check parent objects (that have this object as an aspect)
	return find_instance_up(to, o, this);
}

Object* CompositeType::find_instance_down(const DerivedType* to, Object* o, const DerivedType* avoid) const {
	// First check the base type
	Object* result = base_type()->cast(to, o);
	if (result != nullptr) return result;
	
	// Then do breadth-first search of aspects
	size_t offset = base_type()->size();
	for (auto& aspect: aspects_) {
		Object* aspect_object = reinterpret_cast<Object*>(reinterpret_cast<byte*>(o) + offset);
		
		const CompositeType* composite_aspect = dynamic_cast<const CompositeType*>(aspect);
		if (composite_aspect != nullptr) {
			if (composite_aspect != avoid) {
				Object* result = composite_aspect->find_instance_down(to, aspect_object, nullptr);
				if (result != nullptr) return result;
			}
		} else {
			Object* result = aspect->cast(to, aspect_object);
			if (result != nullptr) return result;
		}
		offset += aspect->size();
	}
	
	return nullptr; // not found
}

Object* CompositeType::find_instance_up(const DerivedType* to, Object* object, const DerivedType* avoid) const {
	Object* o = object->find_parent();
	if (o != nullptr) {
		const DerivedType* t = o->object_type();
		const CompositeType* ct = dynamic_cast<const CompositeType*>(t);
		if (ct != nullptr) {
			return ct->cast(to, o, this);
		} else {
			return t->cast(to, o);
		}
	}
	return nullptr;
}

Object* CompositeType::find_self_up(Object* object) const {
	Object* o = object;
	ssize_t offset = o->object_offset();
	byte* memory = reinterpret_cast<byte*>(o);
	while (true) {
		if (o->object_type() == this) return o;
		if (offset == 0) break;
		memory -= offset;
		o = reinterpret_cast<Object*>(memory);
		offset = o->object_offset();
	}
	return nullptr;
}


void CompositeType::construct(byte* place, IUniverse& universe) const {
	base_type()->construct(place, universe);
	Object* obj = reinterpret_cast<Object*>(place);
	obj->set_object_type__(this);
	size_t offset = base_type()->size();
	for (auto aspect: aspects_) {
		aspect->construct(place + offset, universe);
		Object* subobject = reinterpret_cast<Object*>(place + offset);
		subobject->set_object_offset__(offset);
		subobject->set_object_id(aspect->name()); // might be renamed later by deserialization
		offset += aspect->size();
	}
	ASSERT(offset == size_);
}

void CompositeType::destruct(byte* place, IUniverse& universe) const {
	Object* obj = reinterpret_cast<Object*>(place);
	size_t offset = base_type()->size();
	for (auto aspect: aspects_) { // TODO: Consider doing this backwards?
		aspect->destruct(place + offset, universe);
		offset += aspect->size();
	}
	base_type()->destruct(place, universe);
	ASSERT(offset == size_);
}

void CompositeType::deserialize(byte* place, const ArchiveNode& node, IUniverse& universe) const {
	ASSERT(frozen_);
	base_type()->deserialize(place, node, universe);
	
	const ArchiveNode& aspect_array = node["aspects"];
	if (aspect_array.is_array()) {
		size_t offset = base_type()->size();
		size_t sz = aspect_array.array_size();
		for (size_t i = 0; i < aspect_array.array_size(); ++i) {
			const ArchiveNode& aspect_node = aspect_array[i];
			aspects_[i]->deserialize(place + offset, aspect_node, universe);
			Object* subobject = reinterpret_cast<Object*>(place + offset);
			subobject->set_object_offset__(offset);
			offset += aspects_[i]->size();
		}
		ASSERT(offset == size_);
	}
}

void CompositeType::serialize(const byte* place, ArchiveNode& node, IUniverse& universe) const {
	ASSERT(frozen_);
	base_type()->serialize(place, node, universe);
	node["class"] = base_type()->name();
	
	size_t offset = base_type()->size();
	ArchiveNode& aspect_array = node["aspects"];
	for (auto aspect: aspects_) {
		ArchiveNode& aspect_node = aspect_array.array_push();
		aspect->serialize(place + offset, aspect_node, universe);
		offset += aspect->size();
	}
	ASSERT(offset == size_);
}