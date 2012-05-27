#pragma once
#ifndef BAG_HPP_LRAVL9CJ
#define BAG_HPP_LRAVL9CJ

#include "base/array.hpp"

class BagMemoryHandler {
public:
	byte* allocate();
	void deallocate(byte*);
	void clear();
	
	BagMemoryHandler(size_t element_size) : element_size_(element_size), head_(nullptr), current_(nullptr), free_list_(nullptr) {}
	BagMemoryHandler(BagMemoryHandler&& other) : element_size_(other.element_size_), head_(other.head_), current_(other.current_), free_list_(other.free_list_) {
		other.head_ = nullptr; other.current_ = nullptr; other.free_list_ = nullptr;
	}
	~BagMemoryHandler() { clear(); }
private:
	struct PageHeader {
		PageHeader* next;
		byte* begin;
		byte* current;
		byte* end;
	};
	
	PageHeader* allocate_page();
	void deallocate_page(PageHeader* memory);
	
	const size_t element_size_;
	PageHeader* head_;
	PageHeader* current_;
	byte** free_list_;
};

template <typename T>
class Bag {
public:
	static const size_t ElementSize = sizeof(T) < sizeof(byte*) ? sizeof(byte*) : sizeof(T); // make sure there's room for the implicit freelist
	
	Bag() : memory_(ElementSize) {}
	Bag(Bag<T>&& other);
	~Bag() { clear(); }
	
	template <typename... Args>
	T* allocate(Args&&...);
	void deallocate(T* ptr);
	
	void clear();
private:
	BagMemoryHandler memory_;
};

template <typename T>
Bag<T>::Bag(Bag<T>&& other) : memory_(std::move(other.memory_)) {
}

template <typename T>
template <typename... Args>
T* Bag<T>::allocate(Args&&... args) {
	byte* p = memory_.allocate();
	return ::new(p) T(std::forward<Args>(args)...);
}

template <typename T>
void Bag<T>::deallocate(T* ptr) {
	// TODO: check that ptr is in bag!
	ptr->~T();
	memory_.deallocate(reinterpret_cast<byte*>(ptr));
}

template <typename T>
void Bag<T>::clear() {
	memory_.clear();
}

template <typename T, typename Container = Array<T*>>
class ContainedBag {
public:
	ContainedBag() {}
	ContainedBag(ContainedBag<T>&& other) : bag_(std::move(other.bag_)), elements_(std::move(other.elements_)) {}
	~ContainedBag() { clear(); }
	
	typedef typename Container::iterator iterator;
	typedef typename Container::const_iterator const_iterator;
	
	template <typename... Args>
	T* allocate(Args&&... args) {
		T* ptr = bag_.allocate(std::forward<Args>(args)...);
		elements_.push_back(ptr);
		return ptr;
	}
	
	void deallocate(iterator it) {
		bag_.deallocate(*it);
		elements_.erase(it);
	}
	
	iterator begin() { return elements_.begin(); }
	iterator end() { return elements_.end(); }
	const_iterator begin() const { return elements_.begin(); }
	const_iterator end() const { return elements_.end(); }
	size_t size() const { return elements_.size(); }
	
	void clear() {
		for (auto it: elements_) {
			bag_.deallocate(it);
		}
		elements_.clear();
		bag_.clear();
	}
private:
	Bag<T> bag_;
	Container elements_;
};

#endif /* end of include guard: BAG_HPP_LRAVL9CJ */
