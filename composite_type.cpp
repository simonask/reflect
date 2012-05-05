#include "composite_type.hpp"

Object* CompositeType::cast(const DerivedType* to, Object* o) const {
	Object* result = find_instance_down(to, o);
	if (result != nullptr) return result;
	return find_instance_up(to, o);
}

Object* CompositeType::find_instance_down(const DerivedType* to, Object* o, const DerivedType* avoid) const {
	// Breadth-first search
	size_t offset = sizeof(Object);
	byte* memory = reinterpret_cast<byte*>(o);
	for (auto& it: aspects_) {
		if (it == to && it != avoid) {
			return reinterpret_cast<Object*>(memory + offset);
		}
		offset += it->size();
	}
	offset = sizeof(Object);
	for (auto& it: aspects_) {
		const CompositeType* aspect = dynamic_cast<const CompositeType*>(it);
		if (aspect != nullptr && aspect != avoid) {
			Object* result = aspect->find_instance_down(to, reinterpret_cast<Object*>(memory + offset));
			if (result != nullptr) return result;
		}
		offset += it->size();
	}
	return nullptr;
}

Object* CompositeType::find_instance_up(const DerivedType* to, Object* object) const {
	Object* o = object;
	ssize_t offset = o->object_offset();
	byte* memory = reinterpret_cast<byte*>(o);
	const DerivedType* come_from = this;
	while (true) {
		if (o->object_type() == to) {
			return o;
		} else {
			const CompositeType* comp = dynamic_cast<const CompositeType*>(o->object_type());
			if (comp != nullptr) {
				Object* r = comp->find_instance_down(to, o, come_from);
				if (r != nullptr) {
					return r;
				}
			}
		}
		if (offset == 0) break;
		memory -= offset;
		o = reinterpret_cast<Object*>(memory);
		come_from = o->object_type();
		offset = o->object_offset();
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


void CompositeType::construct(byte* place) const {
	Object* obj = ::new(place) Object;
	obj->set_object_type__(this);
	size_t offset = sizeof(Object);
	for (auto aspect: aspects_) {
		aspect->construct(place + offset);
		Object* subobject = reinterpret_cast<Object*>(place + offset);
		subobject->set_object_offset__(offset);
		offset += aspect->size();
	}
}

void CompositeType::destruct(byte* place) const {
	Object* obj = reinterpret_cast<Object*>(place);
	size_t offset = sizeof(Object);
	for (auto aspect: aspects_) { // TODO: Consider doing this backwards?
		aspect->destruct(place + offset);
		offset += aspect->size();
	}
	obj->~Object();
}

void CompositeType::deserialize(byte* place, const ArchiveNode& node) const {
	assert(frozen_);
	Object* base = reinterpret_cast<Object*>(place);
	base->set_object_type__(this);
	base->set_object_offset__(0);
	
	const ArchiveNode& aspect_array = node["aspects"];
	if (aspect_array.is_array()) {
		size_t offset = sizeof(Object);
		byte* p = place;
		size_t sz = aspect_array.array_size();
		for (size_t i = 0; i < aspect_array.array_size(); ++i) {
			const ArchiveNode& aspect_node = aspect_array[i];
			aspects_[i]->deserialize(p + offset, aspect_node);
			Object* subobject = reinterpret_cast<Object*>(p + offset);
			subobject->set_object_offset__(offset);
			offset += aspects_[i]->size();
		}
	}
}

void CompositeType::serialize(const byte* place, ArchiveNode& node) const {
	assert(frozen_);
	const Object* base = reinterpret_cast<const Object*>(place);
	
	size_t offset = sizeof(Object);
	const byte* p = place;
	ArchiveNode& aspect_array = node["aspects"];
	for (auto aspect: aspects_) {
		ArchiveNode& aspect_node = aspect_array.array_push();
		aspect->serialize(p + offset, aspect_node);
		offset += aspect->size();
	}
}