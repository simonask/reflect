#pragma once
#ifndef SERIALIZE_HPP_37QGG4TA
#define SERIALIZE_HPP_37QGG4TA

#include "serialization/archive.hpp"
#include "object/struct_type.hpp"

template <typename T>
void serialize(const T& object, ArchiveNode& node, IUniverse& universe) {
	const byte* memory = reinterpret_cast<const byte*>(&object);
	get_type(object)->serialize(memory, node, universe);
}

#endif /* end of include guard: SERIALIZE_HPP_37QGG4TA */
