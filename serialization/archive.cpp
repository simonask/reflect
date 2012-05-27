#include "serialization/archive.hpp"
#include "object/objectptr.hpp"
#include "serialization/serialize.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include "type/type_registry.hpp"
#include "object/composite_type.hpp"
#include "object/universe.hpp"
#include "serialization/deserialize_object.hpp"

void Archive::serialize(ObjectPtr<> object, IUniverse& universe) {
	::serialize(*object, root(), universe);
	for (auto ref: serialize_references) {
		ref->perform(universe);
	}
	serialize_references.clear();
}

ObjectPtr<> Archive::deserialize(IUniverse& universe) {
	const ArchiveNode& n = root();
	ObjectPtr<> ptr = deserialize_object(root(), universe);
	
	for (auto it: deserialize_references) {
		it->perform(universe);
	}
	for (auto it: deserialize_signals) {
		it->perform(universe);
	}
	
	return ptr;
	
	if (!n.is_empty()) {
		std::string cls;
		if (!n["class"].get(cls)) {
			std::cerr << "ERROR: Object without class.\n";
			return nullptr;
		}
		std::string id;
		if (!n["id"].get(id)) {
			std::cerr << "WARNING: Object without id.\n";
			id = cls;
		}
		
		const ObjectTypeBase* struct_type = TypeRegistry::get(cls);
		if (struct_type == nullptr) {
			std::cerr << "ERROR: Type '" << cls << "' is not registered.\n";
			return nullptr;
		}
		
		const DerivedType* type = struct_type;
		const ArchiveNode& aspects = n["aspects"];
		if (aspects.is_array()) {
			CompositeType* t = new CompositeType(id + "Type", struct_type);
			
			for (size_t i = 0; i < aspects.array_size(); ++i) {
				const ArchiveNode& aspect = aspects[i];
				std::string aspect_cls;
				if (!aspect["class"].get(aspect_cls)) {
					std::cerr << "ERROR: Aspect without class.\n";
					return nullptr;
				}
				const ObjectTypeBase* aspect_type = TypeRegistry::get(aspect_cls);
				if (aspect_type != nullptr) {
					t->add_aspect(aspect_type);
				} else {
					std::cerr << "ERROR: Aspect type '" << aspect_cls << "' is not registered.\n";
					return nullptr;
				}
			}
			
			t->freeze();
			type = t;
		}
		
		ObjectPtr<> ptr = universe.create_object(type, id);
		if (ptr->object_id() != id) {
			std::cerr << "WARNING: Object '" << id << "' was renamed to '" << ptr->object_id() << "' because of a collision.\n";
		}
		type->deserialize(reinterpret_cast<byte*>(ptr.get()), n, universe);
		
		
		for (auto it: deserialize_references) {
			it->perform(universe);
		}
		for (auto it: deserialize_signals) {
			it->perform(universe);
		}
		
		return ptr;
	}
	return nullptr;
}