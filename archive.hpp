#pragma once
#ifndef ARCHIVE_HPP_A0L9H8RE
#define ARCHIVE_HPP_A0L9H8RE

#include "basic.hpp"
#include "object.hpp"
#include "type.hpp"

#include <string>
#include <vector>
#include <map>
#include <ostream>

struct DeserializeReferenceBase;
struct SerializeReferenceBase;
struct Archive;

struct ArchiveNode {
	enum Type {
		Empty,
		Array,
		Map,
		Integer,
		Float,
		String,
	};
	
	bool is_empty() const { return type_ == Empty; }
	bool is_array() const { return type_ == Array; }
	bool is_map() const { return type_ == Map; }
	Type type() const { return type_; }
	
	bool get(float32&) const;
	bool get(float64&) const;
	bool get(int8&) const;
	bool get(int16&) const;
	bool get(int32&) const;
	bool get(int64&) const;
	bool get(uint8&) const;
	bool get(uint16&) const;
	bool get(uint32&) const;
	bool get(uint64&) const;
	bool get(std::string&) const;
	void set(float32);
	void set(float64);
	void set(int8);
	void set(int16);
	void set(int32);
	void set(int64);
	void set(uint8);
	void set(uint16);
	void set(uint32);
	void set(uint64);
	void set(std::string);
	
	const ArchiveNode& operator[](size_t idx) const;
	ArchiveNode& operator[](size_t idx);
	const ArchiveNode& operator[](const std::string& key) const;
	ArchiveNode& operator[](const std::string& key);
	
	ArchiveNode& array_push();
	size_t array_size() const { return array_.size(); }
	
	template <typename T>
	ArchiveNode& operator=(T value) {
		this->set(value);
		return *this;
	}
	
	virtual ~ArchiveNode() {}
	virtual void write(std::ostream& os) const = 0;
	
	template <typename T>
	void register_reference_for_deserialization(T& reference) const;
	template <typename T>
	void register_reference_for_serialization(const T& reference);
protected:
	explicit ArchiveNode(Archive& archive, Type t = Empty) : archive_(archive), type_(t) {}
protected:
	Archive& archive_;
	Type type_;
	// TODO: Use an 'any'/'variant' type for the following:
	std::map<std::string, ArchiveNode*> map_;
	std::vector<ArchiveNode*> array_;
	std::string string_value;
	union {
		int64 integer_value;
		float64 float_value;
	};
	
	void clear(ArchiveNode::Type new_node_type = Empty);
	template <typename T, typename U>
	bool get_value(T& v, Type value_type, const U& value) const;
};

struct Archive {
	virtual ArchiveNode& root() = 0;
	virtual const ArchiveNode& root() const = 0;
	virtual void write(std::ostream& os) const = 0;
	virtual const ArchiveNode& operator[](const std::string& key) const = 0;
	virtual ArchiveNode& operator[](const std::string& key) = 0;
	virtual ArchiveNode* make(ArchiveNode::Type type = ArchiveNode::Empty) = 0;
	virtual const ArchiveNode& empty() const = 0;
	
	void serialize(Object* object) {
		byte* memory = (byte*)object;
		get_type(object)->serialize(memory, root());
		post_serialization();
		serialize_references.clear();
	}
	
	void register_reference_for_deserialization(DeserializeReferenceBase* ref) { deserialize_references.push_back(ref); }
	void register_reference_for_serialization(SerializeReferenceBase* ref) { serialize_references.push_back(ref); }
private:
	std::vector<DeserializeReferenceBase*> deserialize_references;
	std::vector<SerializeReferenceBase*> serialize_references;
	
	void post_serialization() const;
};


inline void ArchiveNode::set(float32 f) {
	clear(Float);
	float_value = f;
}

inline void ArchiveNode::set(float64 f) {
	clear(Float);
	float_value = f;
}

inline void ArchiveNode::set(int8 n) {
	clear(Integer);
	integer_value = n;
}
inline void ArchiveNode::set(int16 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(int32 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(int64 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint8 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint16 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint32 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint64 n) {
	clear(Integer);
	integer_value = n;
}

inline void ArchiveNode::set(std::string s) {
	clear(String);
	string_value = std::move(s);
}

template <typename T, typename U>
bool ArchiveNode::get_value(T& out_value, ArchiveNode::Type value_type, const U& value) const {
	if (type() == value_type) {
		out_value = value;
		return true;
	}
	return false;
}

inline bool ArchiveNode::get(float32& v) const {
	return get_value(v, Float, float_value);
}

inline bool ArchiveNode::get(float64& v) const {
	return get_value(v, Float, float_value);
}

inline bool ArchiveNode::get(int8& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(int16& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(int32& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(int64& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(uint8& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(uint16& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(uint32& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(uint64& v) const {
	return get_value(v, Integer, integer_value);
}

inline bool ArchiveNode::get(std::string& s) const {
	return get_value(s, String, string_value);
}

inline void ArchiveNode::clear(ArchiveNode::Type new_type) {
	map_.clear();
	array_.clear();
	string_value = "";
	integer_value = 0;
	type_ = new_type;
}

inline const ArchiveNode& ArchiveNode::operator[](size_t idx) const {
	assert(type() == Array);
	if (idx >= array_.size()) {
		return archive_.empty();
	}
	return *array_[idx];
}

inline ArchiveNode& ArchiveNode::operator[](size_t idx) {
	if (type() != Array) {
		clear(Array);
	}
	if (idx < array_.size()) {
		return *array_[idx];
	} else {
		array_.reserve(idx+1);
		while (array_.size() < idx+1) { array_.push_back(archive_.make()); }
		return *array_[idx];
	}
}

inline const ArchiveNode& ArchiveNode::operator[](const std::string& key) const {
	assert(type() == Map);
	auto it = map_.find(key);
	if (it == map_.end()) {
		return archive_.empty();
	} else {
		return *it->second;
	}
}

inline ArchiveNode& ArchiveNode::operator[](const std::string& key) {
	if (type() != Map) {
		clear(Map);
	}
	auto it = map_.find(key);
	if (it == map_.end()) {
		ArchiveNode* n = archive_.make();
		map_[key] = n;
		return *n;
	} else {
		return *it->second;
	}
}

inline ArchiveNode& ArchiveNode::array_push() {
	if (type() != Array) {
		clear(Array);
	}
	ArchiveNode* n = archive_.make();
	array_.push_back(n);
	return *n;
}

struct Universe;

struct DeserializeReferenceBase {
	virtual ~DeserializeReferenceBase() {}
	DeserializeReferenceBase(std::string object_id) : object_id_(object_id) {}
	virtual void perform(Universe&) = 0;
protected:
	Object* get_object(const Universe&) { return nullptr; /* TODO*/ }
	std::string object_id_;
};

template <typename T>
struct DeserializeReference : DeserializeReferenceBase {
public:
	typedef typename T::PointeeType PointeeType;
	
	DeserializeReference(std::string object_id, T& reference) : DeserializeReferenceBase(object_id), reference_(reference) {}
	void perform(Universe& universe) {
		Object* object_ptr = get_object(universe);
		if (object_ptr == nullptr) {
			// TODO: Warn about non-existing object ID.
		}
		PointeeType* ptr = aspect_cast<PointeeType>(object_ptr);
		if (ptr == nullptr) {
			// TODO: Warn about type mismatch.
		}
		reference_ = ptr;
	}
private:
	T& reference_;
};

template <typename T>
void ArchiveNode::register_reference_for_deserialization(T& reference) const {
	std::string id;
	if (get(id)) {
		archive_.register_reference_for_deserialization(new DeserializeReference<T>(id, reference));
	}
}

struct SerializeReferenceBase {
	virtual ~SerializeReferenceBase() {}
	SerializeReferenceBase(ArchiveNode& node) : node_(node) {}
	virtual void perform(Universe&) = 0;
protected:
	std::string get_unique_id(Universe&, Object* object);
	ArchiveNode& node_;
};

template <typename T>
struct SerializeReference : SerializeReferenceBase {
	typedef typename T::PointeeType PointeeType;
	
	SerializeReference(ArchiveNode& node, const T& reference) : SerializeReferenceBase(node), reference_(reference) {}
	void perform(Universe& universe) {
		node_.set(get_unique_id(universe, reference_.get()));
	}
private:
	const T& reference_;
};

template <typename T>
void ArchiveNode::register_reference_for_serialization(const T& reference) {
	archive_.register_reference_for_serialization(new SerializeReference<T>(*this, reference));
}

#endif /* end of include guard: ARCHIVE_HPP_A0L9H8RE */
