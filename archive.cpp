#include "archive.hpp"
#include <sstream>
#include <iomanip>

struct Universe {
	std::map<const Object*, std::string> unique_ids;
	std::map<const Type*, int> type_counters;
	
	std::string get_name_for_type(const Type* type) {
		auto it = type_counters.find(type);
		int n = 0;
		if (it != type_counters.end()) {
			n = it->second;
		}
		
		std::stringstream ss;
		ss << type->name() << std::setw(2) << std::setfill('0') << n;
		type_counters[type] = n+1;
		return ss.str();
	}
	
	std::string get_unique_id(const Object* obj) {
		auto it = unique_ids.find(obj);
		if (it != unique_ids.end()) {
			return it->second;
		}
		std::string n = get_name_for_type(get_type(obj));
		unique_ids[obj] = n;
		return n;
	}
};

void Archive::post_serialization() const {
	Universe universe;
	for (auto ref: serialize_references) {
		ref->perform(universe);
	}
}

std::string SerializeReferenceBase::get_unique_id(Universe& universe, Object* object) {
	return universe.get_unique_id(object);
}