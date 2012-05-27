SOURCES = $(wildcard base/*.cpp) $(wildcard serialization/*.cpp) $(wildcard type/*.cpp) $(wildcard object/*.cpp) *.cpp

CXXFLAGS = -O0 -g -std=c++11 -stdlib=libc++ -Wall -Werror -Wno-unused

all:
	clang++.svn $(CXXFLAGS) $(SOURCES) -I. -o aspect
	#clang++ $(CXXFLAGS) *.cpp -S
	#clang++ -S -emit-llvm $(CXXFLAGS) *.cpp
	#clang++ -S -emit-ast $(CXXFLAGS) *.cpp