# 000b: Simple Integer Arithmetic & Polynomial Eval

## Problem Statement

Implement three basic integer arithmetic functions in `solution.hpp`:

1. `int64_t add_i64(int64_t a, int64_t b)`:
   - Return $a + b$.
2. `int64_t eval_quadratic(int64_t a, int64_t b, int64_t c, int64_t x)`:
   - Evaluate the quadratic polynomial $f(x) = a x^2 + b x + c$.
3. `int64_t sum_first_n(int64_t n)`:
   - For $n \ge 0$, return the sum $\sum_{k=1}^{n} k = \frac{n(n+1)}{2}$.
   - If $n \le 0$, return $0$.
