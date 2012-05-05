CXXFLAGS = -O0 -g -std=c++11 -stdlib=libc++ -fno-exceptions -Wall -Werror -Wno-unused

all:
	clang++ $(CXXFLAGS) *.cpp -o aspect
	#clang++ -S -emit-llvm $(CXXFLAGS) *.cpp
	#clang++ -S -emit-ast $(CXXFLAGS) *.cpp