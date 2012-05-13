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
	
	explicit operator bool() const { return get() != nullptr; }
	
	template <typename Functor>
	bool otherwise(Functor functor) {
		bool b = *this;
		if (!b) functor();
		return b;
	}
private:
	template <typename U, typename R, typename Functor> friend struct MaybeIfImpl;
	
	const T* get() const { return ptr_; }
	T* get() { return ptr_; }
	T* operator->() { return get(); }
	const T* operator->() const { return get(); }
	T& operator*() { return *get(); }
	const T& operator*() const { return *get(); }
	
	
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
