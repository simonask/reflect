#include "object/signal.hpp"
#include "object/struct_type.hpp"
#include <sstream>

std::string SignalTypeBase::build_signal_name(const Array<const Type*>& signature) {
	std::stringstream ss;
	ss << "Signal<";
	for (size_t i = 0; i < signature.size(); ++i) {
		ss << signature[i]->name();
		if (i+1 != signature.size()) {
			ss << ", ";
		}
	}
	ss << ">";
	return ss.str();
}