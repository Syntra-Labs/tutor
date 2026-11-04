# 038: 72-EDO Microtonal Frequency & Harmonic Consonance

## Problem Statement

As an acoustic alternative to BPE tokenization (`tokenizer/README.md`), characters with code $c \ge 0$ are mapped directly onto a **72-EDO** (72 equal divisions of the octave, 16.67 cents per step) microtonal pitch-class ring $\mathbb{Z}_{72}$ anchored at Middle C ($f_0 = 261.63\text{ Hz}$):

1. **72-EDO Carrier Frequency:**
   $$f(c) = 261.63 \cdot 2^{(c \bmod 72) / 72.0}$$
2. **Rhythmic Duration Schedule (in seconds):**
   Given playback speed `master_speed > 0` (default $1.0$):
   $$d(c, \text{master\_speed}) = \frac{45 + ((5c) \bmod 6) \cdot 20}{1000.0 \cdot \text{master\_speed}}$$
3. **Circular Pitch-Class Distance on $\mathbb{Z}_{72}$:**
   Given two character codes $c_1, c_2$, let $p_1 = c_1 \bmod 72$ and $p_2 = c_2 \bmod 72$. Their geodesic step distance on the cyclic octave ring $\mathbb{Z}_{72}$ is:
   $$\Delta_{72}(c_1, c_2) = \min\bigl(|p_1 - p_2|,\; 72 - |p_1 - p_2|\bigr) \in [0, 36]$$

## Mathematical Law / Invariant

- **Octave Periodicity:** $f(c + 72k) = f(c)$ for all $k \in \mathbb{N}$, and $f(0) = 261.63\text{ Hz}$, $f(36) = 261.63 \cdot \sqrt{2}\text{ Hz}$ (exact tritone).
- **Cyclic Metric Space on $\mathbb{Z}_{72}$:** $\Delta_{72}$ is symmetric, satisfies the triangle inequality, and is bounded above by the antipodal tritone distance $36$.
- **6-Permuted Duration Cycle:** Since $\gcd(5, 6) = 1$, as $c \bmod 6$ ranges over $\{0, 1, 2, 3, 4, 5\}$, $(5c) \bmod 6$ visits every duration tier $\{45, 65, 85, 105, 125, 145\}\text{ ms}$ bijectively.

## Function Signature

```cpp
double freq_72edo(uint32_t char_code);
double note_duration_sec(uint32_t char_code, double master_speed = 1.0);
uint32_t pitch_class_distance_72edo(uint32_t c1, uint32_t c2);
```

## Complexity Target

- **Time:** $O(1)$ per character.
- **Space:** $O(1)$.
