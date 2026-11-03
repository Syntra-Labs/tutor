# 052: Order-64 Spinor Group Closure & Cayley Table

## Problem Statement

The 15 bivector matrices $B_{ij} \in \mathcal{C}\ell(0,6)$ generate a finite non-abelian spinor subgroup of order $64 = \{\pm L_S : S \subseteq \{1,\dots,6\}, |S| \text{ even}\}$. Every element of this group is an $8 \times 8$ **signed coordinate permutation matrix** (each row has a single non-zero entry $\pm 1$), allowing each operator to be compressed into an 8-byte array `axes[s][r] = col | (val < 0 ? 8 : 0)` and applied to 8D vectors with zero multiplications.

## Mathematical Law / Invariant

1. **BFS Group Enumeration:** Starting with `states = {I_8}` and a FIFO queue initialized with $I_8$, pop `cur` and for each of the 15 bivector matrices `bm` in lexicographical order, if `nxt = mat_mul(cur, bm)` has not been seen, append it to `states` and push to the queue. Exactly **64 states** are discovered (`identity = 0`).
2. **Compressed Signed-Axis Encoding:** For state $s \in [0, 63]$ and row $r \in [0, 7]$, if `states[s][r][c] != 0`:
   $$\mathrm{axes}[s][r] = c \mid (\mathrm{states}[s][r][c] < 0 \mathrel{?} 8 : 0)$$
   Applying state $s$ to $x \in \mathbb{Z}^8$ computes $y[d] = (\mathrm{axes}[s][d] \mathrel{\&} 8) \mathrel{?} -x[\mathrm{axes}[s][d] \mathrel{\&} 7] : x[\mathrm{axes}[s][d] \mathrel{\&} 7]$.
3. **Cayley Table & Inverses:** `compose[a][b]` stores the state index of `mat_mul(states[a], states[b])`, and `inverse[a]` stores the unique $b$ with `compose[a][b] == 0`.

## Function Signature

```cpp
SpinorGroup64 build_spinor_group_64();
void spinor_action(const SpinorGroup64& grp, uint8_t state_id, const int32_t x[8], int32_t y[8]);
```

## Complexity Target

- **Time:** $O(1)$ per `spinor_action` (8 table lookups + conditional sign flips, zero multiplications).
- **Space:** $O(1)$ (`SpinorGroup64` fits in $< 5\text{ KB}$ of L1 cache).

## Source

- [manifold.cpp](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L233-L284)
