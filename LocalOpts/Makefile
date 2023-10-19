all: LocalOpts.so

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -g -O0

%.so: %.o
	#$(CXX) -dylib -flat_namespace -shared $^ -o $@
	#$(CXX) -dylib -shared $^  -fPIC -o $@
    clang++ -shared -o LocalOpts.so LocalOpts.cpp `llvm-config --cxxflags` -fPIC -Wall
clean:
	rm -f *.o *~ *.so
