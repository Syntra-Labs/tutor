# tutor

> **"A Coding Tutor Rather Than a Coding Agent."**  
> 74 Atomic Math, Geometry, Hyperdimensional Computing & Systems Programming Problem Sets (`001`–`074`) with Prerequisite Skill-Tree DAG, Neovim WASM Studio, Live C++20 Judge, and $0 GitHub Pages Autolab Scoreboard.

### [**Launch Live Neovim WASM Studio & Judge -> `https://syntra-labs.github.io/tutor/`**](https://syntra-labs.github.io/tutor/)

- **In-Browser Neovim WASM (`nvim-asyncify.wasm`)**: Edit `solution.hpp` directly in the browser with full Vim motions, `:w` / `:Run` / `<Space>r` to compile and execute against the locked C++20 harness (`-O3 -std=c++20`), and automatic Autolab Scoreboard ranking (`Score DESC`, `Cycles/Op ASC`, `Bytes ASC`).

---

## 1. Overview

`tutor` is a local-first and Git-native practice harness for systems programming, geometric algebra, exact arithmetic, information theory, and neurosymbolic theorem proving.

- **74 Atomic Problem Sets Across 8 Progressive Tracks**: Every problem isolates **one atomic mathematical law, operator, or data structure primitive** (10–35 lines of C++20 code) arranged in a strict **Prerequisite DAG** (`depends_on`).
- **Locked Test Harnesses & Cycle/Byte Profiling**: `tutor test` always compiles your solution against the locked `problems/<id>/cpp/harness.cpp` inside an isolated sandbox and measures **Score**, **Cycles/Op**, and **Effective Code Bytes**.
- **Autolab Multi-Metric Scoreboard (`colspec`)**: Solutions are ranked using CMU Autolab's multi-column lexicographical comparator (`Score DESC`, `Cycles/Op ASC`, `Bytes ASC`, with earliest-timestamp tie-breaking and `zero_all_scoreboards` suppression on failed tests).
- **$0 Public Judge & GitHub Pages Dashboard**: Uses the battle-tested two-stage GitHub Actions Pull Request pattern (`grade-pr.yml` + `update-scoreboard.yml`) to verify submission isolation on free GitHub runners and publish live rankings + the interactive Prerequisite DAG to GitHub Pages (`docs/index.html`).

---

## 2. Quick Start

### CLI Workflow
```bash
# 1. Browse all 74 problems grouped by Track with live DAG unlock status (✓ SOLVED, ○ READY, 🔒 LOCKED)
python3 tutor.py list

# 2. Visualize the 8-Track Prerequisite Skill-Tree DAG
python3 tutor.py dag

# 3. Pick an unlocked problem and scaffold its solution stub in ~/tutor-workspace/<id>/
python3 tutor.py pick 002

# 4. Run the locked C++20 test harness and view your Autolab Scoreboard metrics
python3 tutor.py test 002

# 5. View the Global or Per-Problem Autolab Scoreboard
python3 tutor.py scoreboard
python3 tutor.py scoreboard -p 002

# 6. Stage a verified solution for the public GitHub Pages Scoreboard
python3 tutor.py submit 002 --user <your-github-username> --update-board
```

---

## 3. The 8 Progressive Tracks (`001`–`074`)

See [`ROADMAP_MATH_AND_ML.md`](ROADMAP_MATH_AND_ML.md) for the full Mermaid dependency graph and mathematical specifications:

1. **Track 1 (`001`–`012`)**: Bit-Parallelism, Cellular Automata (Rules 184, 30, 110) & Balanced Ternary SWAR Logic
2. **Track 2 (`013`–`020`)**: Hyperdimensional Computing (VSA), Bipolar Accumulators & 8-Register Bit-Sliced Carry-Save Adders
3. **Track 3 (`021`–`027`)**: Streaming Information Theory, Dirichlet Byte-Markov Surprisal & Phase-Spike Demarcation
4. **Track 4 (`028`–`038`)**: Exact Algebraic Rings ($\mathbb{Z}[\phi]$), Dyadic Rationals (`U256`, `Dy<P>`), Multiplier-Free LUT Serving & Prime Zeta Codebooks
5. **Track 5 (`039`–`049`)**: Spherical Manifolds ($S^2, S^3$), Quaternion Rotors, Hopf Fibration ($S^3 \to S^2$) & Hamilton Adjoint Gates
6. **Track 6 (`050`–`058`)**: Octonion Fano Plane, Clifford Algebra $\mathcal{C}\ell(0,6)$, Order-64 Spinor Group, $E_8$ Lattices & 600-Cell Icosians
7. **Track 7 (`059`–`066`)**: Sub-Linear Geometric Routing, Helix Cascades, Vicsek/Kuramoto Flock Reads & Ternary Merkle Object Store
8. **Track 8 (`067`–`074`)**: Symbolic ASTs, Reversible Alpha Canonicalization, Non-Commutative Tree Rotors, Geodesic A* Proving & Exact Simplex

---

## 4. Public GitHub Pages Scoreboard & Two-Stage CI Judge

To host the public leaderboard and skill tree for **$0**:
1. Enable **GitHub Pages** in your repository settings pointing to the `main` branch `/docs` folder.
2. When a contributor opens a Pull Request modifying `submissions/<github-username>/<problem-id>/solution.hpp`:
   - **Workflow 1 (`.github/workflows/grade-pr.yml`)** runs in an unprivileged `pull_request` sandbox, verifies that the PR *only* modifies files inside `submissions/<github-username>/`, runs the locked test harness from `main`, and uploads `scoreboard_entries.json`.
   - **Workflow 2 (`.github/workflows/update-scoreboard.yml`)** triggers via `workflow_run`, reduces the new scores using Autolab's `colspec` rules (`tutor/scoreboard.py`), and commits the updated `docs/data/leaderboard.json`.
