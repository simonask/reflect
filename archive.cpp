#include "archive.hpp"
#include <sstream>
#include <iomanip>

ArchiveNode& ArchiveNode::array_push() {
	if (type() != Array) {
		clear(Array);
	}
	ArchiveNode* n = archive_.make();
	array_.push_back(n);
	return *n;
}
