CXXFLAGS = -O0 -g -std=c++11 -stdlib=libc++ -Wall -Werror -Wno-unused

all:
	clang++ $(CXXFLAGS) *.cpp -o aspect
	#clang++ $(CXXFLAGS) *.cpp -S
	#clang++ -S -emit-llvm $(CXXFLAGS) *.cpp
	#clang++ -S -emit-ast $(CXXFLAGS) *.cpp