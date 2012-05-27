#include "serialization/archive_node.hpp"
#include "serialization/archive.hpp"
#include "object/universe.hpp"
#include "object/objectptr.hpp"

ArchiveNode& ArchiveNode::array_push() {
	if (type() != Type::Array) {
		clear(Type::Array);
	}
	ArchiveNode* n = archive_.make();
	array_.push_back(n);
	return *n;
}

const ArchiveNode& ArchiveNode::operator[](size_t idx) const {
	ASSERT(type() == Type::Array);
	if (idx >= array_.size()) {
		return archive_.empty();
	}
	return *array_[idx];
}

ArchiveNode& ArchiveNode::operator[](size_t idx) {
	if (type() != Type::Array) {
		clear(Type::Array);
	}
	if (idx < array_.size()) {
		return *array_[idx];
	} else {
		array_.reserve(idx+1);
		while (array_.size() < idx+1) { array_.push_back(archive_.make()); }
		return *array_[idx];
	}
}

const ArchiveNode& ArchiveNode::operator[](const std::string& key) const {
	ASSERT(type() == Type::Map);
	return *find_or(map_, key, &archive_.empty());
}

ArchiveNode& ArchiveNode::operator[](const std::string& key) {
	if (type() != Type::Map) {
		clear(Type::Map);
	}
	auto it = map_.find(key);
	if (it == map_.end()) {
		ArchiveNode* n = archive_.make();
		map_[key] = n;
		return *n;
	} else {
		return *it->second;
	}
}

void ArchiveNode::register_reference_for_deserialization_impl(DeserializeReferenceBase* ref) const {
	archive_.register_reference_for_deserialization(ref);
}

void ArchiveNode::register_reference_for_serialization_impl(SerializeReferenceBase* ref) {
	archive_.register_reference_for_serialization(ref);
}

void ArchiveNode::register_signal_for_deserialization_impl(DeserializeSignalBase* sig) const {
	archive_.register_signal_for_deserialization(sig);
}

Object* DeserializeReferenceBase::get_object(IUniverse& universe) const {
	return universe.get_object(object_id_).get();
}

std::string SerializeReferenceBase::get_id(const IUniverse& universe, Object* obj) const {
	return universe.get_id(obj);
}

Object* DeserializeSignalBase::get_object(const IUniverse& universe) const {
	return universe.get_object(receiver_id_).get();
}

const SlotAttributeBase* DeserializeSignalBase::get_slot(Object* object) const {
	const DerivedType* type = get_type(object);
	return type->get_slot_by_name(slot_id_);
}