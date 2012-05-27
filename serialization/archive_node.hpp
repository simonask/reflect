#pragma once
#ifndef ARCHIVE_NODE_HPP_EP8GSONT
#define ARCHIVE_NODE_HPP_EP8GSONT

#include <string>
#include <map>
#include <iostream>

#include "serialization/archive_node_type.hpp"
#include "type/type.hpp"

struct Archive;
struct DeserializeReferenceBase;
struct SerializeReferenceBase;
struct DeserializeSignalBase;
struct IUniverse;
struct SlotAttributeBase;
struct DerivedType;

struct ArchiveNode {
	typedef ArchiveNodeType::Type Type;
	
	bool is_empty() const { return type_ == Type::Empty; }
	bool is_array() const { return type_ == Type::Array; }
	bool is_map() const { return type_ == Type::Map; }
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
	void clear() { clear(Type::Empty); }
	
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
	template <typename T>
	void register_signal_for_deserialization(T* signal, std::string receiver_id, std::string slot_id) const;
protected:
	explicit ArchiveNode(Archive& archive, Type t = Type::Empty) : archive_(archive), type_(t) {}
protected:
	Archive& archive_;
	Type type_;
	// TODO: Use an 'any'/'variant' type for the following:
	std::map<std::string, ArchiveNode*> map_;
	Array<ArchiveNode*> array_;
	std::string string_value;
	union {
		int64 integer_value;
		float64 float_value;
	};
	
	void clear(ArchiveNodeType::Type new_node_type);
	template <typename T, typename U>
	bool get_value(T& v, Type value_type, const U& value) const;
	
	void register_reference_for_deserialization_impl(DeserializeReferenceBase* ref) const;
	void register_reference_for_serialization_impl(SerializeReferenceBase* ref);
	void register_signal_for_deserialization_impl(DeserializeSignalBase* sig) const;
};

inline void ArchiveNode::set(float32 f) {
	clear(Type::Float);
	float_value = f;
}

inline void ArchiveNode::set(float64 f) {
	clear(Type::Float);
	float_value = f;
}

inline void ArchiveNode::set(int8 n) {
	clear(Type::Integer);
	integer_value = n;
}
inline void ArchiveNode::set(int16 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(int32 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(int64 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint8 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint16 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint32 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(uint64 n) {
	clear(Type::Integer);
	integer_value = n;
}

inline void ArchiveNode::set(std::string s) {
	clear(Type::String);
	string_value = std::move(s);
}

template <typename T, typename U>
bool ArchiveNode::get_value(T& out_value, ArchiveNodeType::Type value_type, const U& value) const {
	if (type() == value_type) {
		out_value = value;
		return true;
	}
	return false;
}

inline bool ArchiveNode::get(float32& v) const {
	return get_value(v, Type::Float, float_value);
}

inline bool ArchiveNode::get(float64& v) const {
	return get_value(v, Type::Float, float_value);
}

inline bool ArchiveNode::get(int8& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(int16& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(int32& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(int64& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(uint8& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(uint16& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(uint32& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(uint64& v) const {
	return get_value(v, Type::Integer, integer_value);
}

inline bool ArchiveNode::get(std::string& s) const {
	return get_value(s, Type::String, string_value);
}

inline void ArchiveNode::clear(ArchiveNodeType::Type new_type) {
	map_.clear();
	array_.clear();
	string_value = "";
	integer_value = 0;
	type_ = new_type;
}

struct DeserializeReferenceBase {
	virtual ~DeserializeReferenceBase() {}
	DeserializeReferenceBase(std::string object_id) : object_id_(object_id) {}
	virtual void perform(IUniverse&) = 0;
protected:
	std::string object_id_;
	Object* get_object(IUniverse&) const;
};

template <typename T>
struct DeserializeReference : DeserializeReferenceBase {
public:
	typedef typename T::PointeeType PointeeType;
	
	DeserializeReference(std::string object_id, T& reference) : DeserializeReferenceBase(object_id), reference_(reference) {}
	void perform(IUniverse& universe) {
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
		register_reference_for_deserialization_impl(new DeserializeReference<T>(id, reference));
	}
}

struct SerializeReferenceBase {
	virtual ~SerializeReferenceBase() {}
	SerializeReferenceBase(ArchiveNode& node) : node_(node) {}
	virtual void perform(const IUniverse&) = 0;
protected:
	ArchiveNode& node_;
	std::string get_id(const IUniverse&, Object*) const;
};

template <typename T>
struct SerializeReference : SerializeReferenceBase {
	typedef typename T::PointeeType PointeeType;
	
	SerializeReference(ArchiveNode& node, const T& reference) : SerializeReferenceBase(node), reference_(reference) {}
	void perform(const IUniverse& universe) {
		if (reference_ != nullptr) {
			node_.set(get_id(universe, reference_.get()));
		} else {
			node_.clear();
		}
	}
private:
	T reference_;
};

template <typename T>
void ArchiveNode::register_reference_for_serialization(const T& reference) {
	register_reference_for_serialization_impl(new SerializeReference<T>(*this, reference));
}

struct DeserializeSignalBase {
public:
	virtual void perform(const IUniverse&) const = 0;
protected:
	DeserializeSignalBase(std::string receiver, std::string slot) : receiver_id_(std::move(receiver)), slot_id_(std::move(slot)) {}
	std::string receiver_id_;
	std::string slot_id_;
	
	Object* get_object(const IUniverse&) const;
	const SlotAttributeBase* get_slot(Object*) const;
};

template <typename T>
struct DeserializeSignal : DeserializeSignalBase {
	DeserializeSignal(T* signal, std::string receiver, std::string slot) : DeserializeSignalBase(std::move(receiver), std::move(slot)), signal_(signal) {}
	
	void perform(const IUniverse& universe) const {
		Object* object = get_object(universe);
		if (object == nullptr) return;
		const SlotAttributeBase* slot = get_slot(object);
		if (slot == nullptr) return;
		signal_->connect(object, slot);
	}
private:
	T* signal_;
};

template <typename T>
void ArchiveNode::register_signal_for_deserialization(T* signal, std::string receiver, std::string slot) const {
	register_signal_for_deserialization_impl(new DeserializeSignal<T>(signal, std::move(receiver), std::move(slot)));
}

#endif /* end of include guard: ARCHIVE_NODE_HPP_EP8GSONT */
