#pragma once
#ifndef CHILD_LIST_HPP_B25F8VH4
#define CHILD_LIST_HPP_B25F8VH4

#include "base/array.hpp"
#include "object/objectptr.hpp"
#include "base/array_type.hpp"

struct ChildList : Array<ObjectPtr<>> {
	ChildList() {}
	ChildList(const ChildList&) = delete;
	ChildList(ChildList&& other) = default;
	ChildList& operator=(ChildList&& other) = default;
};

struct ChildListType : VariableLengthArrayType<ChildList> {
	ChildListType() : VariableLengthArrayType("ChildList") {}
	virtual ~ChildListType() {}
	void deserialize(ChildList& place, const ArchiveNode& node, IUniverse&) const;
	void serialize(const ChildList& place, ArchiveNode& node, IUniverse&) const override;
};

template <>
struct BuildTypeInfo<ChildList> {
	static const ChildListType* build() {
		static const ChildListType* type = new ChildListType;
		return type;
	}
};

#endif /* end of include guard: CHILD_LIST_HPP_B25F8VH4 */
