#pragma once
#ifndef ARCHIVE_NODE_TYPE_HPP_LDO8RRDW
#define ARCHIVE_NODE_TYPE_HPP_LDO8RRDW

struct ArchiveNodeType {
	enum Type {
		Empty,
		Array,
		Map,
		Integer,
		Float,
		String,
	};
};

#endif /* end of include guard: ARCHIVE_NODE_TYPE_HPP_LDO8RRDW */
