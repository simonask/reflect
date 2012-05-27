#include "type/type.hpp"
#include "base/maybe.hpp"
#include "serialization/archive_node.hpp"

std::string build_maybe_type_name(const Type* inner_type);

template <typename T>
struct MaybeType : TypeFor<Maybe<T>> {
	MaybeType() : name_(build_maybe_type_name(get_type<T>())) {}
	
	void deserialize(Maybe<T>& place, const ArchiveNode&, IUniverse&) const;
	void serialize(const Maybe<T>& place, ArchiveNode&, IUniverse&) const;
	
	const std::string& name() const { return name_; }
	
	const Type* inner_type() const { return get_type<T>(); }
private:
	std::string name_;
};

template <typename T>
void MaybeType<T>::deserialize(Maybe<T>& m, const ArchiveNode& node, IUniverse& universe) const {
	if (!node.is_empty()) {
		T value;
		inner_type()->deserialize(reinterpret_cast<byte*>(&value), node, universe);
		m = std::move(value);
	}
}

template <typename T>
void MaybeType<T>::serialize(const Maybe<T>& m, ArchiveNode& node, IUniverse& universe) const {
	m.map([&](const T& it) {
		inner_type()->serialize(reinterpret_cast<const byte*>(&it), node, universe);
	});
}

template <typename T>
struct BuildTypeInfo<Maybe<T>> {
	static const MaybeType<T>* build() {
		static const MaybeType<T> type;
		return &type;
	}
};
