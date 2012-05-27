#include "object/child_list.hpp"
#include "serialization/deserialize_object.hpp"
#include "serialization/serialize.hpp"

void ChildListType::deserialize(ChildList& list, const ArchiveNode& node, IUniverse& universe) const {
	if (node.is_array()) {
		for (size_t i = 0; i < node.array_size(); ++i) {
			const ArchiveNode& child = node[i];
			ObjectPtr<> ptr = deserialize_object(child, universe);
			if (ptr != nullptr) {
				list.push_back(std::move(ptr));
			}
		}
	}
}

void ChildListType::serialize(const ChildList& list, ArchiveNode& node, IUniverse& universe) const {
	for (auto& child: list) {
		ArchiveNode& child_node = node.array_push();
		::serialize(*child, child_node, universe);
	}
}