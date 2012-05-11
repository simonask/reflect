#include "archive_node.hpp"
#include "archive.hpp"
#include "universe.hpp"
#include "objectptr.hpp"

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
	auto it = map_.find(key);
	if (it == map_.end()) {
		return archive_.empty();
	} else {
		return *it->second;
	}
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

Object* DeserializeReferenceBase::get_object(IUniverse& universe) const {
	return universe.get_object(object_id_).get();
}

std::string SerializeReferenceBase::get_id(const IUniverse& universe, Object* obj) const {
	return universe.get_id(obj);
}