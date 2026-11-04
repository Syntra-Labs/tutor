# 061: Flock Read (Vicsek/Kuramoto Neighbour Consensus)

## Problem Statement

The integer Flock read replaces transformer softmax attention with a hard-ranked Vicsek/Kuramoto alignment rule over the 16 roots of an $E_8$ orbit in $\mathbb{Z}^8$.

Given:
- A list of context nodes $n \in [0, N)$, each with frame root index $f_n \in [0, 15]$, payload root index $p_n \in [0, 15]$, and end position $\text{last}_n \in \mathbb{Z}_{\ge 0}$,
- A query with position $q_{\text{pos}}$, frame root $f_q \in [0, 15]$, and current state root $v_q \in [0, 15]$,
- Neighbour budget $K$ ($1 \le K \le 8$) and ternary gate $g \in \{-1, 0, +1\}$,

implement `flock_read_step`:
1. **Filter & Rank:** Keep only eligible context nodes that end strictly before the query ($\text{last}_n < q_{\text{pos}}$). Rank eligible nodes in ascending lexicographic order of:
   $$\big(d_{L_1}(\text{ROOTS}[f_n], \text{ROOTS}[f_q]),\ d_H(\text{ROOTS}[f_n], \text{ROOTS}[f_q]),\ -\text{last}_n,\ n\big)$$
   where $d_{L_1}(u, v) = \sum_{d=0}^7 |u_d - v_d|$ and $d_H(u, v) = \sum_{d=0}^7 \mathbb{I}[u_d \neq v_d]$. Keep the best $\min(K, N_{\text{elig}})$ nodes.
2. **Carry (Frame Transport):** Transport each kept node's payload $p_n$ into the query frame $f_q$ using the group transport action:
   $$c_n = (p_n \oplus f_n \oplus f_q) \ \& \ 15$$
   so that when $f_n = f_q$, $c_n = p_n$.
3. **Consensus:** Sum the 8D root vectors of all kept transported payloads $\sum_{n \in \text{kept}} \text{ROOTS}[c_n]$ and snap the sum to the nearest of the 16 roots under $L_1$ distance (ties broken by smallest root index $0..15$):
   $$m = \text{snap}_{L_1}\!\left(\sum_{n \in \text{kept}} \text{ROOTS}[c_n]\right)$$
4. **Gated Add:** If $g = 0$ or no eligible nodes exist, return $v_q$. Otherwise compute:
   $$\text{out} = \text{snap}_{L_1}\!\big(\text{ROOTS}[v_q] + g \cdot \text{ROOTS}[m]\big)$$

## Mathematical Law / Invariant

- **Causal Masking:** Any node with $\text{last}_n \ge q_{\text{pos}}$ is never read.
- **Zero-Gate Identity:** When $g = 0$, the output is identically $v_q$ regardless of context.
- **Unanimous Flock Alignment:** If all kept neighbours transport to the same root $r^*$, then the consensus root is $m = r^*$.

## Function Signature

```cpp
uint8_t flock_read_step(
    const std::vector<FlockNode>& context,
    uint32_t query_pos,
    uint8_t f_q,
    uint8_t v_q,
    size_t K,
    int8_t gate_trit,
    uint8_t* out_consensus_root = nullptr
);
```

## Complexity Target

- **Time:** $O(N \log K)$ integer comparisons and additions; zero floating-point operations.
- **Space:** $O(N)$ eligible candidate indices.
