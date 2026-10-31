CXX ?= g++
CXXFLAGS ?= -O3 -std=c++20 -Wall -Wextra -I.

.PHONY: test clean

test: harness_bin
	./harness_bin

harness_bin: harness.cpp solution.hpp
	$(CXX) $(CXXFLAGS) harness.cpp -o harness_bin

clean:
	rm -f harness_bin
