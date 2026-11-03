# 053: 16-Root $E_8$ Orbit Generation & Transport BFS Table

## Problem Statement

Acting with the 64-element $\mathcal{C}\ell(0,6)$ spinor group on the anchor root $r_0 = (2, 2, 0, 0, 0, 0, 0, 0)$ (a scaled $D_8 \subset E_8$ root with $\|r_0\|_1 = 4, \|r_0\|_2^2 = 8$) sweeps out a transitive **16-root orbit** $\mathcal{O}_{16}$ (with stabilizer of order $64 / 16 = 4$). Precomputing the $16 \times 16$ shortest-path BFS table `bfs[src][dst]` over the 15 bivector generators provides $O(1)$ exact group transport between any two roots in $\mathcal{O}_{16}$.

## Mathematical Law / Invariant

1. **Orbit Enumeration (`roots[16]`):**
   Apply all 64 spinor matrices to `anchor = {2, 2, 0, 0, 0, 0, 0, 0}` and collect the 16 unique vectors in a sorted `std::set<std::array<int8_t, 8>>`. Set `roots[0] = anchor` and place the remaining 15 roots in ascending lexicographical order in `roots[1..15]`.
2. **Norm Invariants:** Every $r \in \mathcal{O}_{16}$ satisfies:
   $$\|r\|_1 = \sum_{d=0}^7 |r_d| = 4, \qquad \|r\|_2^2 = \sum_{d=0}^7 r_d^2 = 8, \qquad \|r\|_\infty = 2$$
3. **Transport BFS Table (`bfs[16][16]`):**
   For each `start` $\in [0, 15]$, initialize `paths[start] = 0` (the identity spinor state) and run BFS using the 15 generator states `gen = generators[edge]`. When transitioning from root `from` to `to = find_root(states[gen] * roots[from])` for the first time, record `paths[to] = compose[gen][paths[from]]`.
   **Transport Invariant:** For all $\mathrm{src}, \mathrm{dst} \in [0, 15]$:
   $$\mathrm{action}(\mathrm{bfs}[\mathrm{src}][\mathrm{dst}],\; \mathrm{roots}[\mathrm{src}]) = \mathrm{roots}[\mathrm{dst}]$$

## Function Signature

```cpp
E8Orbit16 build_e8_orbit_16();
```

## Complexity Target

- **Time:** $O(1)$ table construction ($16 \times 16 \times 15$ BFS steps).
- **Space:** $O(1)$ (`bfs` table is $256$ bytes).

## Source

- [manifold.cpp](file:///home/yassine/code/r4-prover/src/cpp/manifold/manifold.cpp#L285-L330)
