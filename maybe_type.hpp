#include "type.hpp"
#include "maybe.hpp"
#include "archive_node.hpp"

std::string build_maybe_type_name(const Type* inner_type);

template <typename T>
struct MaybeType : Type {
	MaybeType() : name_(build_maybe_type_name(get_type<T>())) {}
	
	void deserialize(byte* place, const ArchiveNode&) const override;
	void serialize(const byte* place, ArchiveNode&) const override;
	void construct(byte* place, IUniverse&) const override;
	void destruct(byte* place, IUniverse&) const override;
	
	const std::string& name() const override { return name_; }
	size_t size() const override { return sizeof(Maybe<T>); }
	
	const Type* inner_type() const { return get_type<T>(); }
private:
	std::string name_;
};

template <typename T>
void MaybeType<T>::deserialize(byte* place, const ArchiveNode& node) const {
	Maybe<T>& m = *reinterpret_cast<Maybe<T>*>(place);
	if (!node.is_empty()) {
		T value;
		inner_type()->deserialize(reinterpret_cast<byte*>(&value), node);
		m = std::move(value);
	}
}

template <typename T>
void MaybeType<T>::serialize(const byte* place, ArchiveNode& node) const {
	const Maybe<T>& m = *reinterpret_cast<const Maybe<T>*>(place);
	m.map([&](const T& it) {
		inner_type()->serialize(reinterpret_cast<const byte*>(&it), node);
	});
}

template <typename T>
void MaybeType<T>::construct(byte* place, IUniverse& universe) const {
	::new(place) Maybe<T>;
}

template <typename T>
void MaybeType<T>::destruct(byte* place, IUniverse& universe) const {
	::destruct(reinterpret_cast<byte*>(place));
}

template <typename T>
struct BuildTypeInfo<Maybe<T>> {
	static const MaybeType<T>* build() {
		static const MaybeType<T> type;
		return &type;
	}
};
