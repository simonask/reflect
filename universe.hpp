#pragma once
#ifndef UNIVERSE_HPP_VHU9428R
#define UNIVERSE_HPP_VHU9428R

#include <string>
#include <map>
#include <vector>

#include "object.hpp"

struct DerivedType;

struct IUniverse {
	virtual Object* create_object(const DerivedType* type, std::string id) = 0;
	virtual Object* create_root(const DerivedType* type, std::string id) = 0;
	virtual Object* get_object(const std::string& id) const = 0;
	virtual const std::string& get_id(const Object* object) const = 0;
	virtual bool rename_object(Object* object, std::string new_id) = 0;
	virtual Object* root() const = 0;
	virtual ~IUniverse() {}
	
	template <typename T>
	T* create(std::string id) {
		Object* o = this->create_object(get_type<T>(), std::move(id));
		T* ptr = dynamic_cast<T*>(o);
		assert(ptr != nullptr); // create_object did not create an instance of T.
		return ptr;
	}
};

struct TestUniverse : IUniverse {
	Object* create_object(const DerivedType* type, std::string) override;
	Object* create_root(const DerivedType* type, std::string) override;
	Object* get_object(const std::string& id) const override {
		auto it = object_map_.find(id);
		if (it != object_map_.end()) return it->second;
		return nullptr;
	}
	const std::string& get_id(const Object* object) const override;
	bool rename_object(Object* object, std::string) override;
	Object* root() const override { return root_; }
	
	TestUniverse() : root_(nullptr) {}
	~TestUniverse() { clear(); }
private:
	void clear();
	
	std::map<std::string, Object*> object_map_;
	std::map<const Object*, std::string> reverse_object_map_;
	std::vector<Object*> memory_map_;
	Object* root_;
	std::string empty_id_;
};

#endif /* end of include guard: UNIVERSE_HPP_VHU9428R */
