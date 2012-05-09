#include "object.hpp"
#include "reflect.hpp"
#include "universe.hpp"

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

bool Object::set_object_id(std::string new_id) {
	return universe_->rename_object(this, new_id);
}

const std::string& Object::object_id() const {
	return universe_->get_id(this);
}

BEGIN_TYPE_INFO(Object)
	abstract();
	property(&Object::object_id, &Object::set_object_id, "id", "The unique ID for this object.h");
	// property(&Object::id_, "ID", "The unique ID for this Object.");
END_TYPE_INFO()