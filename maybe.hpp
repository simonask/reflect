#pragma once
#ifndef MAYBE_HPP_8R2MUT0P
#define MAYBE_HPP_8R2MUT0P

#include "basic.hpp"
#include <type_traits>
#include <new>

template <typename T>
class Maybe {
public:
	static const bool IsMoveConstructible = std::is_move_constructible<T>::value;
	static const bool IsMoveAssignable = std::is_move_assignable<T>::value;
	static const bool IsCopyConstructible = std::is_copy_constructible<T>::value;
	static const bool IsCopyAssignable = std::is_copy_assignable<T>::value;
	
	Maybe() : ptr_(nullptr) {}
	Maybe(const Maybe<T>& other);
	Maybe(Maybe<T>&& other);
	Maybe(const T& other);
	Maybe(T&& other);
	~Maybe() { clear(); }
	Maybe<T>& operator=(const Maybe<T>& other);
	Maybe<T>& operator=(Maybe<T>&& other);
	Maybe<T>& operator=(const T& other);
	Maybe<T>& operator=(T&& other);
	
	void clear();
	const T* get() const { return ptr_; }
	T* get() { return ptr_; }
	T* operator->() { return get(); }
	const T* operator->() const { return get(); }
	T& operator*() { return *get(); }
	const T& operator*() const { return *get(); }
	
	explicit operator bool() const { return get() != nullptr; }
private:
	T* ptr_;
	struct Placeholder {
		byte _[sizeof(T)];
	};
	Placeholder memory_;
	
	const T* memory() const { return reinterpret_cast<const T*>(&memory_); }
	T* memory() { return reinterpret_cast<T*>(&memory_); }
	
	template <typename U = T>
	typename std::enable_if<IsCopyConstructibleNonRef<U>::Value && IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (ptr_ != nullptr) {
			*memory() = by_copy;
		} else {
			ptr_ = ::new(memory()) T(by_copy);
		}
	}
	
	template <typename U = T>
	typename std::enable_if<IsCopyConstructibleNonRef<U>::Value && !IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (ptr_ != nullptr) {
			clear();
		}
		ptr_ = ::new(memory()) T(by_copy);
	}
	
	template <typename U = T>
	typename std::enable_if<!IsCopyConstructibleNonRef<U>::Value && IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (ptr_ == nullptr) {
			ptr_ = ::new(memory()) T;
		}
		*memory() = by_copy;
	}
	
	template <typename U = T>
	typename std::enable_if<IsMoveConstructibleNonRef<U>::Value && IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (ptr_ != nullptr) {
			*memory() = std::move(by_move);
		} else {
			ptr_ = ::new(memory()) T(std::move(by_move));
		}
	}
	
	template <typename U = T>
	typename std::enable_if<IsMoveConstructibleNonRef<U>::Value && !IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (ptr_ != nullptr) {
			clear();
		}
		ptr_ = ::new(memory()) T(std::move(by_move));
	}
	
	template <typename U = T>
	typename std::enable_if<!IsMoveConstructibleNonRef<U>::Value && IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (ptr_ == nullptr) {
			ptr_ = ::new(memory()) T;
		}
		*memory() = std::move(by_move);
	}
};

template <typename T>
Maybe<T>::Maybe(const Maybe<T>& other) : ptr_(nullptr) {
	if (other) assign(*other);
}

template <typename T>
Maybe<T>::Maybe(Maybe<T>&& other) : ptr_(nullptr) {
	if (other) assign(std::move(*other));
	other.clear();
}

template <typename T>
Maybe<T>::Maybe(const T& other) : ptr_(nullptr) {
	assign(other);
}

template <typename T>
Maybe<T>::Maybe(T&& other) : ptr_(nullptr) {
	assign(std::move(other));
}

template <typename T>
Maybe<T>& Maybe<T>::operator=(const Maybe<T>& other) {
	if (other) assign(*other);
	else clear();
	return *this;
}

template <typename T>
Maybe<T>& Maybe<T>::operator=(Maybe<T>&& other) {
	if (other) { assign(std::move(*other)); other.clear(); }
	else clear();
	return *this;
}

template <typename T>
Maybe<T>& Maybe<T>::operator=(const T& other) {
	assign(other);
	return *this;
}

template <typename T>
Maybe<T>& Maybe<T>::operator=(T&& other) {
	assign(std::move(other));
	return *this;
}

template <typename T>
void Maybe<T>::clear() {
	if (ptr_ != nullptr) {
		memory()->~T();
		ptr_ = nullptr;
	}
}

#endif /* end of include guard: MAYBE_HPP_8R2MUT0P */
