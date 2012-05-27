#pragma once
#ifndef JSON_ARCHIVE_HPP_4OX35IUJ
#define JSON_ARCHIVE_HPP_4OX35IUJ

#include "serialization/archive.hpp"
#include "serialization/archive_node.hpp"
#include "base/bag.hpp"
#include <map>
#include <string>


struct JSONArchive;

struct JSONArchiveNode : ArchiveNode {
	JSONArchiveNode(JSONArchive& archive, ArchiveNodeType::Type t = ArchiveNodeType::Empty);
	void write(std::ostream& os) const override { write(os, false, 0); }
	void write(std::ostream& os, bool print_inline, int indent) const;
};

struct JSONArchive : Archive {
	JSONArchive();
	ArchiveNode& root() override;
	const ArchiveNode& root() const override;
	void write(std::ostream& os) const override;
	const ArchiveNode& operator[](const std::string& key) const override;
	ArchiveNode& operator[](const std::string& key) override;
	ArchiveNode* make(ArchiveNode::Type t = ArchiveNodeType::Empty) override { return make_internal(t); }
	
	const ArchiveNode& empty() const { return *empty_; }
private:
	friend struct JSONArchiveNode;
	JSONArchiveNode* empty_;
	JSONArchiveNode* root_;
	ContainedBag<JSONArchiveNode> nodes_;
	JSONArchiveNode* make_internal(ArchiveNodeType::Type t = ArchiveNodeType::Empty);
};

inline JSONArchiveNode::JSONArchiveNode(JSONArchive& archive, ArchiveNode::Type t) : ArchiveNode(archive, t) {}

#endif /* end of include guard: JSON_ARCHIVE_HPP_4OX35IUJ */
