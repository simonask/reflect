#include "archive.hpp"
#include "objectptr.hpp"
#include "serialize.hpp"
#include <sstream>
#include <iomanip>

void Archive::serialize(ObjectPtr<> object, IUniverse& universe) {
	::serialize(*object, root());
	for (auto ref: serialize_references) {
		ref->perform(universe);
	}
	serialize_references.clear();
}