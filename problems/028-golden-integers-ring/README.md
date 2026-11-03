# 028: Golden Integer Ring $\mathbb{Z}[\phi]$ Addition & Multiplication

## Problem Statement

The coordinates of the 120 unit icosians and $H_4$ root systems contain the golden ratio $\phi = \frac{1 + \sqrt{5}}{2}$, an irrational algebraic integer satisfying $\phi^2 = \phi + 1$.

To compute with golden coordinates with **zero floating-point rounding**, represent every element of the ring of integers $\mathbb{Z}[\phi] = \{a + b\phi : a, b \in \mathbb{Z}\}$ as an exact integer pair `ZPhi { int64_t a, b; }`.

## Mathematical Law / Invariant

Using the defining minimal polynomial identity $\phi^2 = \phi + 1$:

1. **Addition & Subtraction:**
   $$(a + b\phi) \pm (c + d\phi) = (a \pm c) + (b \pm d)\phi$$
2. **Exact Ring Multiplication:**
   $$(a + b\phi)(c + d\phi) = ac + (ad + bc)\phi + bd\phi^2 = (ac + bd) + (ad + bc + bd)\phi$$
3. **Golden Ratio Identity:**
   Let $\Phi = (0, 1)$. Then $\Phi \cdot \Phi = (1, 1) = 1 + \phi$, and $\Phi \cdot (\Phi - 1) = (0, 1) \cdot (-1, 1) = (1, 0) = 1$.

## Function Signature

```cpp
struct ZPhi {
    int64_t a = 0;
    int64_t b = 0;

    constexpr bool operator==(const ZPhi& o) const = default;
    constexpr ZPhi operator+(const ZPhi& o) const;
    constexpr ZPhi operator-(const ZPhi& o) const;
    constexpr ZPhi operator-() const;
    constexpr ZPhi operator*(const ZPhi& o) const;
};
```

## Complexity Target

- **Time:** $O(1)$ (4 integer multiplications and 3 additions per product, or 3 multiplications via Karatsuba).
- **Space:** $O(1)$ (16 bytes per `ZPhi`).

## Source Reference

- [`golden-integers.md`](file:///home/yassine/code/uor-r4/docs/geometry/golden-integers.md)
