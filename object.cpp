#include "object.hpp"
#include "reflect.hpp"

Object* Object::find_topmost_object() {
	Object* object = this;
	while (object->offset_ != 0) {
		ssize_t offs = object->offset_;
		object = reinterpret_cast<Object*>(reinterpret_cast<byte*>(object) - offs);
	}
	return object;
}

const Object* Object::find_topmost_object() const {
	const Object* object = this;
	while (object->offset_ != 0) {
		ssize_t offs = object->offset_;
		object = reinterpret_cast<const Object*>(reinterpret_cast<const byte*>(object) - offs);
	}
	return object;
}

BEGIN_TYPE_INFO(Object)
	abstract();
	// property(&Object::id_, "ID", "The unique ID for this Object.");
END_TYPE_INFO()