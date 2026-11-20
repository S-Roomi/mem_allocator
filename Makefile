CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

all: build/test_allocator build/benchmark

build:
	mkdir -p build

build/test_allocator: src/test_allocator.cpp include/allocator.hpp | build
	$(CXX) $(CXXFLAGS) src/test_allocator.cpp -o build/test_allocator

build/benchmark: src/benchmark.cpp include/allocator.hpp | build
	$(CXX) $(CXXFLAGS) src/benchmark.cpp -o build/benchmark

test: build/test_allocator
	./build/test_allocator

bench: build/benchmark
	./build/benchmark 100000

clean:
	rm -rf build

.PHONY: all test bench clean
