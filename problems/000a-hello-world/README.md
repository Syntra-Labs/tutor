# 000a: Hello World & String Greeting

## Problem Statement

A quick warmup problem to verify that the editor and C++20 test judge are working end-to-end.

Implement two functions in `solution.hpp`:

1. `std::string hello_world()`:
   - Return the exact string `"Hello, World!"`.
2. `std::string greet(std::string_view name)`:
   - If `name` is empty, return `"Hello, World!"`.
   - Otherwise, return `"Hello, " + std::string(name) + "!"`.
