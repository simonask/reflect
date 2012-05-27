#include "object/universe.hpp"
#include "object/struct_type.hpp"

#include <iomanip>

ObjectPtr<> TestUniverse::create_root(const DerivedType* type, std::string id) {
	clear();
	root_ = create_object(type, std::move(id));
	return root_;
}

ObjectPtr<> TestUniverse::create_object(const DerivedType* type, std::string id) {
	size_t sz = type->size();
	byte* memory = new byte[sz];
	type->construct(memory, *this);
	Object* object = reinterpret_cast<Object*>(memory);
	memory_map_.push_back(object);
	rename_object(object, id);
	return object;
}

bool TestUniverse::rename_object(ObjectPtr<> object, std::string new_id) {
	ASSERT(object->universe() == this);
	
	// erase old name from database
	auto old_it = reverse_object_map_.find(object);
	if (old_it != reverse_object_map_.end()) {
		object_map_.erase(old_it->second);
	}

	// check if new name already exists
	auto it = object_map_.find(new_id);
	bool renamed_exact = true;
	std::string new_name;
	if ((it != object_map_.end()) || (new_id.size() < 2)) {
		// it does, so create a unique name from the requested name
		int n = 1;
		std::string base_name;
		if (new_id.size() >= 2) {
			std::stringstream recognize_number_at_end(new_id.substr(new_id.size()-2, 2));
			if (!(recognize_number_at_end >> n).fail()) {
				base_name = new_id.substr(0, new_id.size()-2);
			} else {
				base_name = std::move(new_id);
			}
			n += 1; // n is set to 0 if recognition failed, otherwise the existing number. Add one. :)
		} else {
			base_name = object->object_type()->name();
		}
		
		// increment n and try the name until we find one that's available
		do {
			std::stringstream create_new_name;
			create_new_name << base_name << std::setw(2) << std::setfill('0') << n;
			new_name = std::move(create_new_name.str());
		} while (object_map_.find(new_name) != object_map_.end());
		
		renamed_exact = false;
	} else {
		new_name = std::move(new_id);
	}
	
	object_map_[new_name] = object;
	reverse_object_map_[object] = std::move(new_name);
	return renamed_exact;
}

const std::string& TestUniverse::get_id(ObjectPtr<const Object> object) const {
	auto it = reverse_object_map_.find(object);
	if (it != reverse_object_map_.end()) {
		return it->second;
	}
	return empty_id_;
}

void TestUniverse::clear() {
	for (auto object: memory_map_) {
		const DerivedType* type = object->object_type();
		type->destruct(reinterpret_cast<byte*>(object), *this);
	}
	// TODO: Test for references?
	object_map_.clear();
	reverse_object_map_.clear();
	memory_map_.clear();
}