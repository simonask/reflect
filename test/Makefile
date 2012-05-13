CXXFLAGS = -O0 -g -I.. -std=c++11 -stdlib=libc++ -Wall -Werror -Wno-unused
CXX = clang++
COMPILE = $(CXX) $(CXXFLAGS)

maybe_test: maybe_test.cpp ../maybe.hpp
	$(COMPILE) -o maybe_test maybe_test.cpp
	./maybe_test

clean:
	rm -f maybe_test

all: maybe_test