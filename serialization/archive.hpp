#pragma once
#ifndef ARCHIVE_HPP_A0L9H8RE
#define ARCHIVE_HPP_A0L9H8RE

#include "base/basic.hpp"
#include "object/object.hpp"
#include "type/type.hpp"
#include "object/objectptr.hpp"
#include "serialization/archive_node_type.hpp"

#include <string>

struct DeserializeReferenceBase;
struct SerializeReferenceBase;
struct DeserializeSignalBase;
struct ArchiveNode;
struct IUniverse;

struct Archive {
	typedef ArchiveNodeType::Type NodeType;
	
	virtual ArchiveNode& root() = 0;
	virtual const ArchiveNode& root() const = 0;
	virtual void write(std::ostream& os) const = 0;
	virtual const ArchiveNode& operator[](const std::string& key) const = 0;
	virtual ArchiveNode& operator[](const std::string& key) = 0;
	virtual ArchiveNode* make(NodeType type = NodeType::Empty) = 0;
	virtual const ArchiveNode& empty() const = 0;
	
	void serialize(ObjectPtr<> object, IUniverse& universe);
	ObjectPtr<> deserialize(IUniverse& universe);
	
	void register_reference_for_deserialization(DeserializeReferenceBase* ref) { deserialize_references.push_back(ref); }
	void register_reference_for_serialization(SerializeReferenceBase* ref) { serialize_references.push_back(ref); }
	void register_signal_for_deserialization(DeserializeSignalBase* sig) {
		deserialize_signals.push_back(sig);
	}
private:
	Array<DeserializeReferenceBase*> deserialize_references;
	Array<SerializeReferenceBase*> serialize_references;
	Array<DeserializeSignalBase*> deserialize_signals;
};

#endif /* end of include guard: ARCHIVE_HPP_A0L9H8RE */
