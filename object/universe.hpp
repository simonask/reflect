#pragma once
#ifndef UNIVERSE_HPP_VHU9428R
#define UNIVERSE_HPP_VHU9428R

#include <string>
#include <map>


#include "object/object.hpp"
#include "object/objectptr.hpp"

struct DerivedType;

struct IUniverse {
	virtual ObjectPtr<> create_object(const DerivedType* type, std::string id) = 0;
	virtual ObjectPtr<> create_root(const DerivedType* type, std::string id) = 0;
	virtual ObjectPtr<> get_object(const std::string& id) const = 0;
	virtual const std::string& get_id(ObjectPtr<const Object> object) const = 0;
	virtual bool rename_object(ObjectPtr<> object, std::string new_id) = 0;
	virtual ObjectPtr<> root() const = 0;
	virtual ~IUniverse() {}
	
	template <typename T>
	ObjectPtr<T> create(std::string id) {
		ObjectPtr<> o = this->create_object(get_type<T>(), std::move(id));
		ObjectPtr<T> ptr = o.cast<T>();
		ASSERT(ptr != nullptr); // create_object did not create an instance of T.
		return ptr;
	}
};

struct TestUniverse : IUniverse {
	ObjectPtr<> create_object(const DerivedType* type, std::string) override;
	ObjectPtr<> create_root(const DerivedType* type, std::string) override;
	ObjectPtr<> get_object(const std::string& id) const override {
		return find_or(object_map_, id, nullptr);
	}
	const std::string& get_id(ObjectPtr<const Object> object) const override;
	bool rename_object(ObjectPtr<> object, std::string) override;
	ObjectPtr<> root() const override { return root_; }
	
	TestUniverse() : root_(nullptr) {}
	~TestUniverse() { clear(); }
private:
	void clear();
	
	std::map<std::string, ObjectPtr<>> object_map_;
	std::map<ObjectPtr<const Object>, std::string> reverse_object_map_;
	Array<Object*> memory_map_;
	ObjectPtr<> root_;
	std::string empty_id_;
};

#endif /* end of include guard: UNIVERSE_HPP_VHU9428R */
