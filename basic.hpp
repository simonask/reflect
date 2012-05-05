#pragma once
#ifndef BASIC_HPP_S0NRU03V
#define BASIC_HPP_S0NRU03V

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <memory>

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;
typedef float float32;
typedef double float64;
typedef uint8 byte;

template <typename T, typename NameGetter>
struct HasMember {
	typedef char MatchedReturnType;
	typedef long UnmatchedReturnType;
	
	template <typename C>
	static MatchedReturnType f(typename NameGetter::template Check<C>*);
	
	template <typename C>
	static UnmatchedReturnType f(...);
public:
	static const bool Value = (sizeof(f<T>(0)) == sizeof(MatchedReturnType)); 
};

#endif /* end of include guard: BASIC_HPP_S0NRU03V */
