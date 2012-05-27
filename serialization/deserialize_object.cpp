#include "serialization/deserialize_object.hpp"
#include "serialization/archive_node.hpp"
#include "type/type_registry.hpp"
#include "object/composite_type.hpp"
#include "object/struct_type.hpp"
#include "object/universe.hpp"
#include <memory>

namespace {
	const DerivedType* get_type_from_map(const ArchiveNode& node, std::string& out_error);
	
	const ObjectTypeBase* get_class_from_map(const ArchiveNode& node, std::string& out_error) {
		std::string clsname;
		if (!node["class"].get(clsname)) {
			out_error = "Class not specified.";
			return nullptr;
		}
		const ObjectTypeBase* struct_type = TypeRegistry::get(clsname);
		if (struct_type == nullptr) {
			out_error = "Class '" + clsname + "' not registered.";
			return nullptr;
		}
		return struct_type;
	}
	
	const DerivedType* transform_if_composite_type(const ArchiveNode& node, const ObjectTypeBase* base_type, std::string& out_error) {
		const ArchiveNode& aspects = node["aspects"];
		if (!aspects.is_array()) return base_type;
		if (aspects.array_size() == 0) return base_type;
		
		CompositeType* type = new CompositeType("Composite", base_type);
		for (size_t i = 0; i < aspects.array_size(); ++i) {
			const ArchiveNode& aspect = aspects[i];
			const DerivedType* aspect_type = get_type_from_map(aspect, out_error);
			if (aspect_type == nullptr) {
				return nullptr;
			}
			type->add_aspect(aspect_type);
		}
		type->freeze();
		return type;
	}
	
	const DerivedType* get_type_from_map(const ArchiveNode& node, std::string& out_error) {
		const ObjectTypeBase* struct_type = get_class_from_map(node, out_error);
		if (struct_type != nullptr) {
			return transform_if_composite_type(node, struct_type, out_error);
		}
		return nullptr;
	}
}

ObjectPtr<> deserialize_object(const ArchiveNode& node, IUniverse& universe) {
	if (!node.is_map()) {
		std::cerr << "Expected object, got non-map.\n";
		return nullptr;
	}
	
	std::string error;
	const DerivedType* type = get_type_from_map(node, error);
	if (type == nullptr) {
		std::cerr << "ERROR: " << error << '\n';
		return nullptr;
	}
	
	std::string id;
	if (!node["id"].get(id)) {
		std::cerr << "WARNING: Object without id.\n";
	}
	
	ObjectPtr<> ptr = universe.create_object(type, id);
	if (ptr->object_id() != id) {
		std::cerr << "WARNING: Object '" << id << "' was renamed to '" << ptr->object_id() << "' because of a collision.\n";
	}
	
	type->deserialize(reinterpret_cast<byte*>(ptr.get()), node, universe);
	
	return ptr;
}
