#include "base/maybe_type.hpp"

std::string build_maybe_type_name(const Type* inner_type) {
	std::stringstream ss;
	ss << "Maybe<" << inner_type->name() << '>';
	return ss.str();
}