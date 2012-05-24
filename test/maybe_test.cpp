#include "maybe.hpp"

#if defined(__has_feature) && __has_feature(cxx_lambdas)
#define HAS_LAMBDAS 1
#else
#define HAS_LAMBDAS 0
#endif

struct TrivialType {
	TrivialType(int n = 0) : n(n) {}
	TrivialType(const TrivialType& other) = default;
	TrivialType(TrivialType&& other) = default;
	TrivialType& operator=(const TrivialType&) = default;
	TrivialType& operator=(TrivialType&&) = default;
	int n;
};

struct OnlyCopyConstructible {
	OnlyCopyConstructible(int n = 0) : n(n) {}
	OnlyCopyConstructible(const OnlyCopyConstructible& other) = default;
	OnlyCopyConstructible(OnlyCopyConstructible&&) = delete;
	OnlyCopyConstructible& operator=(const OnlyCopyConstructible&) = delete;
	OnlyCopyConstructible& operator=(OnlyCopyConstructible&&) = delete;
	int n;
};

struct OnlyMoveConstructible {
	OnlyMoveConstructible(int n = 0) : n(n) {}
	//OnlyMoveConstructible(const OnlyMoveConstructible&) = delete;
	OnlyMoveConstructible(OnlyMoveConstructible&& other) { n = other.n; }
	OnlyMoveConstructible& operator=(const OnlyMoveConstructible&) = delete;
	OnlyMoveConstructible& operator=(OnlyMoveConstructible&&) = delete;
	int n;
};

struct OnlyCopyAssignable {
	OnlyCopyAssignable(int n = 0) : n(n) {}
	OnlyCopyAssignable(const OnlyCopyAssignable& other) = delete;
	OnlyCopyAssignable(OnlyCopyAssignable&&) = delete;
	OnlyCopyAssignable& operator=(const OnlyCopyAssignable&) = default;
	OnlyCopyAssignable& operator=(OnlyCopyAssignable&&) = delete;
	int n;
};

struct OnlyMoveAssignable {
	OnlyMoveAssignable(int n = 0) : n(n) {}
	OnlyMoveAssignable(const OnlyMoveAssignable& other) = delete;
	OnlyMoveAssignable(OnlyMoveAssignable&&) = delete;
	OnlyMoveAssignable& operator=(const OnlyMoveAssignable&) = delete;
	OnlyMoveAssignable& operator=(OnlyMoveAssignable&&) = default;
	int n;
};

struct CopyConstructibleAndAssignable {
	CopyConstructibleAndAssignable(int n = 0) : n(n) {}
	CopyConstructibleAndAssignable(const CopyConstructibleAndAssignable& other) = default;
	CopyConstructibleAndAssignable(CopyConstructibleAndAssignable&&) = delete;
	CopyConstructibleAndAssignable& operator=(const CopyConstructibleAndAssignable& other) = default;
	CopyConstructibleAndAssignable& operator=(CopyConstructibleAndAssignable&&) = delete;
	int n;
};

struct MoveConstructibleAndAssignable {
	MoveConstructibleAndAssignable(int n = 0) : n(n) {}
	MoveConstructibleAndAssignable(const MoveConstructibleAndAssignable& other) = delete;
	MoveConstructibleAndAssignable(MoveConstructibleAndAssignable&&) = default;
	MoveConstructibleAndAssignable& operator=(const MoveConstructibleAndAssignable& other) = delete;
	MoveConstructibleAndAssignable& operator=(MoveConstructibleAndAssignable&&) = default;
	int n;
};

struct CopyConstructibleAndMoveAssignable {
	CopyConstructibleAndMoveAssignable(int n = 0) : n(n) {}
	CopyConstructibleAndMoveAssignable(const CopyConstructibleAndMoveAssignable& other) = default;
	CopyConstructibleAndMoveAssignable(CopyConstructibleAndMoveAssignable&&) = delete;
	CopyConstructibleAndMoveAssignable& operator=(const CopyConstructibleAndMoveAssignable& other) = delete;
	CopyConstructibleAndMoveAssignable& operator=(CopyConstructibleAndMoveAssignable&&) = default;
	int n;
};

struct MoveConstructibleAndCopyAssignable {
	MoveConstructibleAndCopyAssignable(int n = 0) : n(n) {}
	MoveConstructibleAndCopyAssignable(const MoveConstructibleAndCopyAssignable& other) = delete;
	MoveConstructibleAndCopyAssignable(MoveConstructibleAndCopyAssignable&&) = default;
	MoveConstructibleAndCopyAssignable& operator=(const MoveConstructibleAndCopyAssignable& other) = default;
	MoveConstructibleAndCopyAssignable& operator=(MoveConstructibleAndCopyAssignable&&) = delete;
	int n;
};

struct CopyAndMoveConstructibleCopyAssignable {
	CopyAndMoveConstructibleCopyAssignable(int n = 0) : n(n) {}
	CopyAndMoveConstructibleCopyAssignable(const CopyAndMoveConstructibleCopyAssignable& other) = default;
	CopyAndMoveConstructibleCopyAssignable(CopyAndMoveConstructibleCopyAssignable&&) = default;
	CopyAndMoveConstructibleCopyAssignable& operator=(const CopyAndMoveConstructibleCopyAssignable& other) = default;
	CopyAndMoveConstructibleCopyAssignable& operator=(CopyAndMoveConstructibleCopyAssignable&&) = delete;
	int n;
};

struct CopyAndMoveConstructibleMoveAssignable {
	CopyAndMoveConstructibleMoveAssignable(int n = 0) : n(n) {}
	CopyAndMoveConstructibleMoveAssignable(const CopyAndMoveConstructibleMoveAssignable& other) = default;
	CopyAndMoveConstructibleMoveAssignable(CopyAndMoveConstructibleMoveAssignable&&) = default;
	CopyAndMoveConstructibleMoveAssignable& operator=(const CopyAndMoveConstructibleMoveAssignable& other) = delete;
	CopyAndMoveConstructibleMoveAssignable& operator=(CopyAndMoveConstructibleMoveAssignable&&) = default;
	int n;
};

struct CopyConstructibleCopyAndMoveAssignable {
	CopyConstructibleCopyAndMoveAssignable(int n = 0) : n(n) {}
	CopyConstructibleCopyAndMoveAssignable(const CopyConstructibleCopyAndMoveAssignable& other) = default;
	CopyConstructibleCopyAndMoveAssignable(CopyConstructibleCopyAndMoveAssignable&&) = delete;
	CopyConstructibleCopyAndMoveAssignable& operator=(const CopyConstructibleCopyAndMoveAssignable& other) = default;
	CopyConstructibleCopyAndMoveAssignable& operator=(CopyConstructibleCopyAndMoveAssignable&&) = default;
	int n;
};

struct MoveConstructibleCopyAndMoveAssignable {
	MoveConstructibleCopyAndMoveAssignable(int n = 0) : n(n) {}
	MoveConstructibleCopyAndMoveAssignable(const MoveConstructibleCopyAndMoveAssignable& other) = delete;
	MoveConstructibleCopyAndMoveAssignable(MoveConstructibleCopyAndMoveAssignable&&) = default;
	MoveConstructibleCopyAndMoveAssignable& operator=(const MoveConstructibleCopyAndMoveAssignable& other) = default;
	MoveConstructibleCopyAndMoveAssignable& operator=(MoveConstructibleCopyAndMoveAssignable&&) = default;
	int n;
};

struct NoDefaultConstructor {
	explicit NoDefaultConstructor(int n) : n(n) {}
	int n;
};


template <typename T>
void run_test(Maybe<T>& m) {
	if (m) exit(1);
	T value(123);
	m = std::move(value);
	if (m.get()->n != 123) exit(1);
	m = std::move(value);
	if (m.get()->n != 123) exit(1);
}

static int regular_function_call_count = 0;

void regular_function_byval(int) {
	regular_function_call_count++;
}

void regular_function_byref(int&) {
	regular_function_call_count++;
}

void regular_function_byconstref(const int&) {
	regular_function_call_count++;
}

void test_maybe_if() {
	Maybe<int> yes = 123;
	Maybe<int> no;
	
#if HAS_LAMBDAS
	// Test call with lambda
	{
		int count = 0;
		maybe_if(yes, [&](int it) { ++count; });
		maybe_if(yes, [&](int& it) { ++count; });
		maybe_if(yes, [&](const int& it) { ++count; });
		ASSERT(count == 3);
		maybe_if(no, [](int) {});
		maybe_if(no, [](int&) {});
		maybe_if(no, [](const int&) {});
		ASSERT(count == 3);
	}
#endif

	// Test call with regular function
	{
		regular_function_call_count = 0;
		maybe_if(yes, regular_function_byval);
		maybe_if(yes, regular_function_byref);
		maybe_if(yes, regular_function_byconstref);
		ASSERT(regular_function_call_count == 3);
		maybe_if(no, regular_function_byval);
		maybe_if(no, regular_function_byref);
		maybe_if(no, regular_function_byconstref);
		ASSERT(regular_function_call_count == 3);
	}
	
	// Test call with function object
	{
		struct ByVal {
			int& count;
			ByVal(int& count) : count(count) {}
			void operator()(int) { count++; }
		};
		
		struct ByRef {
			int& count;
			ByRef(int& count) : count(count) {}
			void operator()(int&) { count++; }
		};
		
		struct ByConstRef {
			int& count;
			ByConstRef(int& count) : count(count) {}
			void operator()(const int&) { count++; }
		};
		
		int count = 0;
		maybe_if(yes, ByVal(count));
		maybe_if(yes, ByRef(count));
		maybe_if(yes, ByConstRef(count));
		ASSERT(count == 3);
		maybe_if(no, ByVal(count));
		maybe_if(no, ByRef(count));
		maybe_if(no, ByConstRef(count));
		ASSERT(count == 3);
	}
	
	// Test return values
	
#if HAS_LAMBDAS
	{
		Maybe<int> m = maybe_if(yes, [](int) { return 1; });
		ASSERT(m.is_set());
		m = maybe_if(no, [](int) { return 1; });
		ASSERT(!m.is_set());
	}
#endif

	{
		struct F {
			int operator()(int) { return 1; }
		};
		
		Maybe<int> m = maybe_if(yes, F());
		ASSERT(m.is_set());
		m = maybe_if(no, F());
		ASSERT(!m.is_set());
	}
	
	// Test boolean return values for void
	
#if HAS_LAMBDAS
	{
		bool b = yes.map([](int) { });
		ASSERT(b);
		b = maybe_if(no, [](int) {});
		ASSERT(!b);
	}
#endif

	{
		struct F {
			void operator()(int) {}
		};
		
		bool b = yes.map(F());
		ASSERT(b);
		b = maybe_if(no, F());
		ASSERT(!b);
	}
	
	// Test non-Maybe option types
	{
		int n = 123;
		int* p = &n;
		auto m = maybe_if(p, [](int it) { return it; });
		ASSERT(m);
		maybe_if(m, [&](int it) { ASSERT(it == n); });
		p = nullptr;
		m = maybe_if(p, [](int it) { return it; });
		ASSERT(!m);
	}
}

int main (int argc, char const *argv[])
{
	{
		Maybe<TrivialType> m;
		m = TrivialType(123);
		TrivialType t(123);
		m = t;
	}
	
	{
		Maybe<OnlyCopyConstructible> m(OnlyCopyConstructible(123));
		m = OnlyCopyConstructible(123);
		OnlyCopyConstructible v(123);
		m = v;
	}
	
	{
		Maybe<OnlyMoveConstructible> m(OnlyMoveConstructible(123));
		m = OnlyMoveConstructible(123);
		OnlyMoveConstructible v(123);
		m = std::move(v);
	}
	
	{
		Maybe<OnlyCopyAssignable> m(OnlyCopyAssignable(123));
		m = OnlyCopyAssignable(123);
		OnlyCopyAssignable v(123);
		m = v;
	}
	
	{
		Maybe<OnlyMoveAssignable> m(OnlyMoveAssignable(123));
		m = OnlyMoveAssignable(123);
		OnlyMoveAssignable v(123);
		m = std::move(v);
	}
	
	{
		Maybe<CopyConstructibleAndAssignable> m(CopyConstructibleAndAssignable(123));
		m = CopyConstructibleAndAssignable(123);
		CopyConstructibleAndAssignable v(123);
		m = v;
	}
	
	{
		Maybe<MoveConstructibleAndAssignable> m(MoveConstructibleAndAssignable(123));
		m = MoveConstructibleAndAssignable(123);
		MoveConstructibleAndAssignable v(123);
		m = std::move(v);
	}
	
	{
		Maybe<CopyConstructibleAndMoveAssignable> m(CopyConstructibleAndMoveAssignable(123));
		m = CopyConstructibleAndMoveAssignable(123);
		CopyConstructibleAndMoveAssignable v(123);
		m = std::move(v);
	}
	
	{
		Maybe<MoveConstructibleAndCopyAssignable> m(MoveConstructibleAndCopyAssignable(123));
		m = MoveConstructibleAndCopyAssignable(123);
		MoveConstructibleAndCopyAssignable v(123);
		m = v;
	}
	
	{
		Maybe<CopyAndMoveConstructibleCopyAssignable> m(CopyAndMoveConstructibleCopyAssignable(123));
		CopyAndMoveConstructibleCopyAssignable v(123);
		Maybe<CopyAndMoveConstructibleCopyAssignable> m2(v);
		Maybe<CopyAndMoveConstructibleCopyAssignable> m3(std::move(v));
		m = v;
	}
	
	{
		Maybe<CopyAndMoveConstructibleMoveAssignable> m(CopyAndMoveConstructibleMoveAssignable(123));
		CopyAndMoveConstructibleMoveAssignable v(123);
		m = std::move(v);
		Maybe<CopyAndMoveConstructibleMoveAssignable> m2 = v;
	}
	
	{
		CopyConstructibleCopyAndMoveAssignable v(123);
		Maybe<CopyConstructibleCopyAndMoveAssignable> m(v);
		m = std::move(v);
	}
	
	{
		MoveConstructibleCopyAndMoveAssignable v(123);
		Maybe<MoveConstructibleCopyAndMoveAssignable> m(std::move(v));
		m = std::move(v);
		m = v;
	}
	
	{
		Maybe<NoDefaultConstructor> m = NoDefaultConstructor(123);
		m = NoDefaultConstructor(123);
		NoDefaultConstructor v(456);
		m = v;
	}
	
	test_maybe_if();
	
	return 0;
}