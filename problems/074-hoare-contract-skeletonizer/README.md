# 074: Hoare Logic Contract Parser & Function Skeletonizer (`lgcc`)

## Problem Statement

To compress source code context by $>70\%$ while preserving complete type signatures and formal Hoare contracts (`crates/lgcc/src/ast/skeletonizer.rs` & `src/cpp/oracle/code_oracle.hpp`), the Contract Skeletonizer extracts `@requires` / `@ensures` annotations and replaces function implementation bodies `{ ... }` with `{ /* TODO */ }`.

Implement two functions:
1. **`std::vector<HoareContractSpec> extract_hoare_contracts(const std::string& source_code)`:**
   - Scan `source_code` line by line.
   - Collect consecutive doc-comment contract annotations of the form:
     - `// @requires: <expr>`
     - `// @ensures: <expr>`
     (ignoring leading whitespace).
   - When the immediately following function signature line is encountered (a line containing `'('` and `')'` that is not a comment), extract the function identifier `fn_name` (the identifier immediately preceding `'('`) and emit a `HoareContractSpec{fn_name, requires_list, ensures_list}`.
2. **`std::string skeletonize_cpp_source(const std::string& source_code)`:**
   - Replace every top-level function body `{ ... }` (ignoring braces inside string literals `"..."` and character literals `'...'`) with `{ /* TODO */ }`, while preserving:
     - All `// @requires:` and `// @ensures:` contract comment lines,
     - `struct` / `class` / `namespace` declarations (`struct Foo { ... };` keeps its declarations, while methods inside or free functions have their bodies replaced by `{ /* TODO */ }`),
     - Balanced nested braces inside function bodies (e.g., `if (...) { for (...) { ... } }` collapses cleanly into a single `{ /* TODO */ }`).

## Mathematical Law / Invariant

- **Signature & Contract Preservation:** Every extracted `HoareContractSpec` and function signature before `{` remains intact in the skeletonized output.
- **Brace Balance Invariant:** Outside string/char literals, the number of `'{'` equals the number of `'}'` in the skeletonized output, and every function body has brace depth reduced to 1 (`{ /* TODO */ }`).

## Function Signature

```cpp
std::vector<HoareContractSpec> extract_hoare_contracts(const std::string& source_code);
std::string skeletonize_cpp_source(const std::string& source_code);
```

## Complexity Target

- **Time:** $O(N)$ single-pass lexical scan over `source_code`.
- **Space:** $O(N)$ output buffer.
