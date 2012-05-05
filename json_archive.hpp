#pragma once
#ifndef JSON_ARCHIVE_HPP_4OX35IUJ
#define JSON_ARCHIVE_HPP_4OX35IUJ

#include "archive.hpp"
#include <deque>
#include <map>
#include <string>
#include <vector>

struct JSONArchive;

struct JSONArchiveNode : ArchiveNode {
	JSONArchiveNode(JSONArchive& archive, ArchiveNode::Type t = ArchiveNode::Empty) : ArchiveNode(t), archive_(archive) {}
	void write(std::ostream& os) const override { write(os, false, 0); }
	void write(std::ostream& os, bool print_inline, int indent) const;
protected:
	ArchiveNode* make(Type t) override;
	const ArchiveNode& empty() const override;
private:
	JSONArchive& archive_;
};

struct JSONArchive : Archive {
	JSONArchive();
	ArchiveNode& root() override;
	const ArchiveNode& root() const override;
	void write(std::ostream& os) const override;
	const ArchiveNode& operator[](const std::string& key) const override;
	ArchiveNode& operator[](const std::string& key) override;
	
	const JSONArchiveNode& empty() const { return *empty_; }
private:
	friend struct JSONArchiveNode;
	JSONArchiveNode* empty_;
	JSONArchiveNode* root_;
	std::deque<JSONArchiveNode> nodes_;
	JSONArchiveNode* make(ArchiveNode::Type = ArchiveNode::Empty);
};

inline ArchiveNode* JSONArchiveNode::make(Type t) {
	return archive_.make(t);
}

inline const ArchiveNode& JSONArchiveNode::empty() const {
	return archive_.empty();
}

#endif /* end of include guard: JSON_ARCHIVE_HPP_4OX35IUJ */
