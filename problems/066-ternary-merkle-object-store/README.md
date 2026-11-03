# 066: Canonical Ternary Merkle Object Store (`fold` & `range`)

## Problem Statement

The durable-memory exact byte store (`uor-r4-object-store`) cuts an arbitrary byte buffer into 32-byte zero-padded atoms and folds them into a deduplicated 3-child (ternary) Merkle DAG where repeated content is stored once and any byte slice `[start, end)` can be reconstructed and authenticated by visiting only intersecting subtrees.

Implement `TernaryObjectStore`:
1. **`Manifest fold(const std::vector<uint8_t>& bytes)`:**
   - If `bytes` is empty, return an empty manifest (`byte_len = 0`, `root_id = 0`).
   - Slice `bytes` into $N = \lceil \text{bytes.size()} / 32 \rceil$ ordered 32-byte atoms, zero-padding the final atom if needed.
   - Each leaf record has `scale = 0`, `leaf_bytes[32]`, and content fingerprint `id = hash_leaf(leaf_bytes)`. Insert into the deduplicated `records` map (so identical 32-byte atoms share one record).
   - While the current level has more than 1 node, group consecutive nodes into triads of 3 (if the final group has 1 or 2 nodes, pad missing child slots with `0ULL` and do not increment span for `0ULL` children). Each parent record has:
     $$\text{scale} = \text{child\_scale} + 1, \qquad \text{span\_bytes} = \sum_{c \neq 0} \text{child}[c].\text{span\_bytes}$$
     and fingerprint `id = hash_internal(scale, children[0], children[1], children[2])`. Store deduplicated in `records`.
   - Return `Manifest{bytes.size(), root_id}`.
2. **`std::vector<uint8_t> rebuild(const Manifest& m) const`:**
   - Reconstruct the exact byte vector of length `m.byte_len` by traversing from `m.root_id`.
3. **`std::vector<uint8_t> range(const Manifest& m, size_t start, size_t end, size_t* out_visited_nodes) const`:**
   - Reconstruct the exact slice `bytes[start..end)` by descending only into children whose byte interval intersects `[start, end)`, incrementing `*out_visited_nodes` for each DAG node visited during the traversal.

## Mathematical Law / Invariant

- **Exact Round-Trip Obligation:** $\text{rebuild}(\text{fold}(b)) = b$ byte-for-byte for every input $b$.
- **Strict Scale Monotonicity:** Every internal node at scale $s$ has non-empty children solely at scale $s - 1$, forbidding cycles.
- **Deduplication Compression:** Folding a buffer of $3^k \times 32$ identical bytes produces at most $k + 1$ unique records in `records` (one per scale $0..k$).
- **Sub-Linear Range Read:** Reading a 23-byte slice inside a multi-kilobyte tree touches $O(\log_3 N)$ nodes rather than all $N$ leaves.

## Function Signature

```cpp
class TernaryObjectStore {
public:
    Manifest fold(const std::vector<uint8_t>& data);
    std::vector<uint8_t> rebuild(const Manifest& m) const;
    std::vector<uint8_t> range(const Manifest& m, size_t start, size_t end, size_t* out_visited_nodes = nullptr) const;
    size_t unique_record_count() const;
};
```

## Complexity Target

- **Time:** `fold` in $O(N)$ hashes; `range` in $O(\log_3 N + (end - start)/32)$ node visits.
- **Space:** $O(U)$ unique records in the deduplicated DAG.

## Source Reference

- [object-store.md](file:///home/yassine/code/uor-r4/docs/mechanisms/object-store.md#L20-L35)
