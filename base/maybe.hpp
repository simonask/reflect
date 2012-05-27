#pragma once
#ifndef MAYBE_HPP_8R2MUT0P
#define MAYBE_HPP_8R2MUT0P

#include "base/basic.hpp"
#include <type_traits>
#include <new>

template <typename M, typename ReturnType> struct MaybeIfImpl;

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
	
	// For use with decltype(...)
	T& infer_value_type() { ASSERT(false); return *((T*)nullptr); }
	const T& infer_value_type() const { ASSERT(false); return *((T*)nullptr); }
	
	template <typename Functor>
	bool otherwise(Functor functor) {
		bool b = is_set();
		if (!b) functor();
		return b;
	}
	template <typename Functor>
	bool otherwise(Functor functor) const {
		bool b = is_set();
		if (!b) functor();
		return b;
	}
	
	// Same as 'maybe_if', but with FP terminology a la Scala.
	template <typename Functor>
	auto map(Functor functor)
	-> typename MaybeIfImpl<Maybe<T>, decltype(functor(infer_value_type()))>::ResultType {
		return MaybeIfImpl<Maybe<T>, decltype(functor(infer_value_type()))>::maybe_if(*this, functor);
	}
	template <typename Functor>
	auto map(Functor functor) const
	-> typename MaybeIfImpl<const Maybe<T>, decltype(functor(infer_value_type()))>::ResultType {
		return MaybeIfImpl<const Maybe<T>, decltype(functor(infer_value_type()))>::maybe_if(*this, functor);
	}
private:
	template <typename M, typename ReturnType> friend struct MaybeIfImpl;
	
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
	const byte* is_set_ptr() const { return reinterpret_cast<const byte*>(&memory_) + sizeof(T); }
	byte* is_set_ptr() { return reinterpret_cast<byte*>(&memory_) + sizeof(T); }
	
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
	
	operator bool() const { return value_; }
};

template <typename M, typename ReturnType>
struct MaybeIfImpl;

template <typename M>
struct MaybeIfImpl<M, void> {
	typedef BooleanHolder ResultType;
	
	template <typename Functor>
	static BooleanHolder maybe_if(M& m, Functor function) {
		if (m) { function(*m); return true; }
		return false;
	}
};

template <typename M, typename ReturnType>
struct MaybeIfImpl {
	typedef Maybe<typename RemoveMaybe<ReturnType>::Type> ResultType;
	
	template <typename Functor>
	static ResultType maybe_if(M& m, Functor function) {
		if (m) return function(*m);
		return ResultType();
	}
};


template <typename M, typename Functor>
auto maybe_if(M& m, Functor function)
-> typename MaybeIfImpl<M, decltype(function(m.infer_value_type()))>::ResultType
{
	return MaybeIfImpl<M, decltype(function(m.infer_value_type()))>::maybe_if(m, function);
}

template <typename M, typename Functor>
auto maybe_if(M& m, Functor function)
-> typename MaybeIfImpl<M, decltype(function(*m))>::ResultType
{
	return MaybeIfImpl<M, decltype(function(*m))>::maybe_if(m, function);
}

template <typename OutputStream, typename T>
OutputStream& operator<<(OutputStream& os, const Maybe<T>& m) {
	maybe_if(m, [&](const T& it) { os << it; }).otherwise([&]() { os << "(none)"; });
	return os;
}

#endif /* end of include guard: MAYBE_HPP_8R2MUT0P */
