# 004: Cellular Automaton Rule 184 Boundary Demarcation

## Specification & Contract

Cellular Automaton Rule 184 is a deterministic 1D traffic flow model that conserves particle count. In unformatted streaming code, Rule 184 acts as a natural boundary demarcator without requiring a programming language grammar.

### Transition Rule:
For cell state $x_i \in \{0, 1\}$ with neighbors $x_{i-1}, x_{i+1}$:
$$x_i^{t+1} = (x_{i-1} \land \neg x_i) \lor (x_i \land x_{i+1})$$

### Requirements:
Implement a 64-bit SWAR (SIMD Within A Register) Rule 184 step function:
```cpp
uint64_t step_rule184(uint64_t state, uint64_t left_in, uint64_t right_in);
```
- Conserves total set bits (particles) across transitions with periodic or streamed boundaries.
- Returns the demarcated state after $T$ steps.
