# 067: Recursive Descent Symbolic Expression Parser & Evaluator

## Problem Statement

Foundational to symbolic engines (`lawful/symbolic.hpp`) is parsing infix mathematical expressions into a binary Abstract Syntax Tree (`Tree`) and evaluating it recursively while respecting operator precedence and associativity.

Implement a recursive-descent parser and evaluator over infix strings containing floating-point/integer numbers, whitespace, parentheses `( ... )`, and binary operators `+`, `-`, `*`, `/`, `^`:
1. **Grammar & Precedence:**
   - **Expression (`+`, `-`):** Left-associative addition and subtraction (`parseExpression`):
     $$\text{Expr} \to \text{Term} \ ((\text{'+'} \mid \text{'-'}) \ \text{Term})^*$$
   - **Term (`*`, `/`):** Left-associative multiplication and division (`parseTerm`):
     $$\text{Term} \to \text{Power} \ ((\text{'*'} \mid \text{'/'}) \ \text{Power})^*$$
   - **Power (`^`):** Right-associative exponentiation (`parseFactor`):
     $$\text{Power} \to \text{Primary} \ (\text{'\^'} \ \text{Power})?$$
   - **Primary:** Unsigned/signed numeric literal (e.g. `42`, `3.5`) or parenthesized `( Expr )`.
2. **AST Representation & Evaluation:**
   - Construct a `sym::Tree` where internal nodes store the operator symbol (`"+"`, `"-"`, `"*"`, `"/"`, `"^"`) in `data` with `left` and `right` child pointers, and leaf nodes store the number string in `data`.
   - Implement `double evalExpression(const Tree* root)` to recursively evaluate the AST.

## Mathematical Law / Invariant

- **Precedence Law:** `eval("2 + 3 * 4") == 14.0`, whereas `eval("(2 + 3) * 4") == 20.0`.
- **Left-Associativity of `-` and `/`:** `eval("10 - 3 - 2") == 5.0` and `eval("24 / 6 / 2") == 2.0`.
- **Right-Associativity of `^`:** `eval("2 ^ 3 ^ 2") == 512.0` ($2^{(3^2)} = 2^9 = 512$, not $(2^3)^2 = 64$).

## Function Signature

```cpp
namespace sym {
    void parseExpression(Tree& expression, int& p, const std::string& str);
    double evalExpression(const Tree* expression);
    void deallocateTree(Tree& root);
}
```

## Complexity Target

- **Time:** $O(N)$ single-pass recursive descent in expression length $N$.
- **Space:** $O(T)$ AST nodes.
