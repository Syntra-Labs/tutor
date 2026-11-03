# 068: Reversible Alpha & Premise IR Canonicalizer

## Problem Statement

To make theorem proving invariant to surface variable names (`x`, `y`, `n`, `h_assoc`) and open-vocabulary lemma names, the IR Canonicalizer (`src/ir/canonicalizer.py`) normalizes Lean 4 declarations and proof states into a canonical turnstile IR while recording a reversible bijection $\sigma$.

Implement:
1. **`std::string to_turnstile_state(const std::string& goal_str)`:**
   - Normalize horizontal whitespace (`[ \t]+` $\to$ `" "`) and strip empty lines.
   - If `"⊢"` is already present, return the normalized string.
   - Otherwise, strip any trailing `":= by"` or `":="`, extract all binder groups `(names : Type)`, `{names : Type}`, `[names : Type]` in order into context lines `"names : Type"`, and extract the target after the final `":"` as `"⊢ Target"`.
2. **`CanonicalGoal canonicalize_goal_and_tactic(const std::string& goal_str, const std::string& tactic_str)`:**
   - Convert `goal_str` via `to_turnstile_state`.
   - Scan context lines `names : Type` before `"⊢"` in declaration order. For each identifier token (not in `LEAN_KEYWORDS` and not yet in $\sigma$):
     - Classify as a **hypothesis** (`h0, h1, ...`) if its name starts with `'h'` followed by nothing, digits, `'_'`, or `'\''`, OR if its `Type` contains any relational/logical symbol (`"="`, `"<"`, `">"`, `"≤"`, `"≥"`, `"≠"`, `"∧"`, `"∨"`, `"↔"`, `"¬"`), OR if its `Type` was previously declared with type `"Prop"`.
     - Otherwise classify as a **variable** (`v0, v1, ...`). Record `sigma[surface] = canon`.
   - Scan `tactic_str` for identifier tokens not in `LEAN_KEYWORDS`, not in $\sigma$, and not already canonical (`v\d+`, `h\d+`, `p\d+`). Assign external premise IDs `p0, p1, ...` in order of first appearance in `tactic_str`.
   - Replace all whole-word identifier occurrences in the turnstile goal and tactic string using $\sigma$.
3. **`std::string invert_sigma(const std::string& canon_tactic, const std::unordered_map<std::string, std::string>& sigma)`:**
   - Apply $\sigma^{-1}$ (`canon -> surface`) to restore the exact original surface identifiers.

## Mathematical Law / Invariant

- **Alpha-Invariance:** Two isomorphic theorems differing only in bound variable/hypothesis names (`theorem t1 (a b : Nat) (hab : a = b) : b = a` vs `theorem t2 (x y : Nat) (hxy : x = y) : y = x`) produce the **exact same** `canon_goal`:
  `v0 v1 : Nat\nh0 : v0 = v1\n⊢ v1 = v0`.
- **Lossless Round-Trip Inversion:** For any surface tactic $T$, `invert_sigma(canonicalize(G, T).canon_tactic, sigma) == T`.

## Function Signature

```cpp
std::string to_turnstile_state(const std::string& goal_str);
CanonicalGoal canonicalize_goal_and_tactic(const std::string& goal_str, const std::string& tactic_str = "");
std::string invert_sigma(const std::string& canon_text, const std::unordered_map<std::string, std::string>& sigma);
```

## Complexity Target

- **Time:** $O(|G| + |T|)$ linear scan in string length.
- **Space:** $O(|\sigma|)$ hash map entries.

## Source Reference

- [canonicalizer.py](file:///home/yassine/code/r4-prover/src/ir/canonicalizer.py)
