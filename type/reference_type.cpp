#include "reference_type.hpp"
#include <sstream>

std::string ReferenceType::build_reference_type_name(std::string base_name, const Type* pointee) {
	std::stringstream ss;
	ss << base_name << '<' << pointee->name() << '>';
	return ss.str();
}