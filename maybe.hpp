#pragma once
#ifndef MAYBE_HPP_8R2MUT0P
#define MAYBE_HPP_8R2MUT0P

#include "basic.hpp"
#include <type_traits>
#include <new>

template <typename T, typename R, typename Functor> struct MaybeIfImpl;

template <typename T>
class Maybe {
public:
	Maybe();
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
	
	bool is_set() const { return *is_set_ptr() != 0; }
	explicit operator bool() const { return is_set(); }
	
	template <typename Functor>
	bool otherwise(Functor functor) {
		bool b = *this;
		if (!b) functor();
		return b;
	}
private:
	template <typename U, typename R, typename Functor> friend struct MaybeIfImpl;
	
	const T* get() const { return is_set() ? memory() : nullptr; }
	T* get() { return is_set() ? memory() : nullptr; }
	T* operator->() { return get(); }
	const T* operator->() const { return get(); }
	T& operator*() { return *get(); }
	const T& operator*() const { return *get(); }
	
	static const size_t StorageSize = sizeof(T) + 1; // T + is_set byte
	static const size_t Alignment = std::alignment_of<T>::value;
	typedef typename std::aligned_storage<StorageSize, Alignment>::type StorageType;
	
	StorageType memory_;
	
	const T* memory() const { return reinterpret_cast<const T*>(&memory_); }
	T* memory() { return reinterpret_cast<T*>(&memory_); }
	const byte* is_set_ptr() const { return reinterpret_cast<const byte*>(&memory_ + sizeof(T)); }
	byte* is_set_ptr() { return reinterpret_cast<byte*>(&memory_ + sizeof(T)); }
	
	void set(bool b) {
		if (b) *is_set_ptr() = 1;
		else   *is_set_ptr() = 0;
	}
	
	template <typename U = T>
	typename std::enable_if<IsCopyConstructibleNonRef<U>::Value && IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (is_set()) {
			*memory() = by_copy;
		} else {
			::new(memory()) T(by_copy);
			set(true);
		}
	}
	
	template <typename U = T>
	typename std::enable_if<IsCopyConstructibleNonRef<U>::Value && !IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (is_set()) {
			clear();
		}
		::new(memory()) T(by_copy);
		set(true);
	}
	
	template <typename U = T>
	typename std::enable_if<!IsCopyConstructibleNonRef<U>::Value && IsCopyAssignableNonRef<U>::Value>::type
	assign(const T& by_copy) {
		if (!is_set()) {
			::new(memory()) T;
			set(true);
		}
		*memory() = by_copy;
	}
	
	template <typename U = T>
	typename std::enable_if<IsMoveConstructibleNonRef<U>::Value && IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (is_set()) {
			*memory() = std::move(by_move);
		} else {
			::new(memory()) T(std::move(by_move));
			set(true);
		}
	}
	
	template <typename U = T>
	typename std::enable_if<IsMoveConstructibleNonRef<U>::Value && !IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (is_set()) {
			clear();
		}
		::new(memory()) T(std::move(by_move));
		set(true);
	}
	
	template <typename U = T>
	typename std::enable_if<!IsMoveConstructibleNonRef<U>::Value && IsMoveAssignableNonRef<U>::Value>::type
	assign(T&& by_move) {
		if (!is_set()) {
			::new(memory()) T;
			set(true);
		}
		*memory() = std::move(by_move);
	}
};

template <typename T>
Maybe<T>::Maybe() {
	set(false);
}

template <typename T>
Maybe<T>::Maybe(const Maybe<T>& other) {
	set(false);
	if (other) assign(*other);
}

template <typename T>
Maybe<T>::Maybe(Maybe<T>&& other) {
	set(false);
	if (other) assign(std::move(*other));
	other.clear();
}

template <typename T>
Maybe<T>::Maybe(const T& other) {
	set(false);
	assign(other);
}

template <typename T>
Maybe<T>::Maybe(T&& other) {
	set(false);
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
	if (is_set()) {
		memory()->~T();
		set(false);
	}
}

template <typename T>
struct RemoveMaybe;
template <typename T>
struct RemoveMaybe<Maybe<T>> {
	typedef T Type;
};
template <typename T>
struct RemoveMaybe {
	typedef T Type;
};

struct BooleanHolder {
	BooleanHolder(bool value) : value_(value) {}
	bool value_;
	
	template <typename Functor>
	bool otherwise(Functor functor) {
		if (!value_) functor();
		return value_;
	}
};

template <typename T, typename Functor>
struct MaybeIfImpl<T, void, Functor> {
	typedef BooleanHolder ResultType;
	
	static BooleanHolder maybe_if(Maybe<T>& maybe, Functor function) {
		if (maybe) { function(*maybe); return true; }
		return false;
	}
	static BooleanHolder maybe_if(const Maybe<T>& maybe, Functor function) {
		if (maybe) { function(*maybe); return true; }
		return false;
	}
};

template <typename T, typename R, typename Functor>
struct MaybeIfImpl {
	typedef typename RemoveMaybe<R>::Type ReturnType;
	typedef Maybe<ReturnType> ResultType;
	
	static ResultType maybe_if(Maybe<T>& maybe, Functor function) {
		if (maybe) return function(*maybe);
		return ResultType();
	}
	
	static ResultType maybe_if(const Maybe<T>& maybe, Functor function) {
		if (maybe) return function(*maybe);
		return ResultType();
	}
};

template <typename T, typename Functor>
struct MaybeIf {
	typedef typename std::result_of<Functor(T&)>::type ReturnType;
	typedef MaybeIfImpl<T, ReturnType, Functor> Impl;
	typedef typename Impl::ResultType ResultType;
	
	static ResultType maybe_if(Maybe<T>& maybe, Functor function) {
		return Impl::maybe_if(maybe, function);
	}
	
	static ResultType maybe_if(const Maybe<T>& maybe, Functor function) {
		return Impl::maybe_if(maybe, function);
	}
};

template <typename T, typename Functor>
typename MaybeIf<T,Functor>::ResultType
maybe_if(Maybe<T>& maybe, Functor function) {
	return MaybeIf<T,Functor>::maybe_if(maybe, function);
}

template <typename T, typename Functor>
typename MaybeIf<T,Functor>::ResultType
maybe_if(const Maybe<T>& maybe, Functor function) {
	return MaybeIf<T,Functor>::maybe_if(maybe, function);
}

#endif /* end of include guard: MAYBE_HPP_8R2MUT0P */
