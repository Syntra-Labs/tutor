# tutor

> **"A Coding Tutor Rather Than a Coding Agent."**  
> Sovereign systems programming problem sets, leetvim-style runner & logical bug compiler (`lgcc`).

---

## 1. Overview

`tutor` is a minimalist, local-first development harness designed for deliberate systems programming practice, contract verification, and algorithmic derivation.

Modeled with the instant ergonomics of tools like **leetvim**, `tutor` provides:
- **Zero-Friction CLI & Neovim Integration**: Pick, code, and test problems entirely inside your sovereign editor without external browsers or accounts.
- **Syntra Labs Problem Sets**: Real-world low-level systems engineering (lock-free concurrency, memory models, geometric hypervectors, O(1) intrusive caches, cellular automata chunking).
- **Problem Stub Generator (`tutor slice` / `lgcc --skeletonize`)**: Tool to strip function bodies into empty problem stubs while keeping signatures and comments intact.
- **Local-First Verification**: Compiles and runs test harnesses locally on your machine with millisecond feedback.
- **Optional Distributed Network Sync**:
  - Pull updated problem sets and regression tests from the upstream git repo or URL (`tutor pull`).
  - Optionally register with a distributed cluster node for automated remote benchmarking and telemetry (`tutor connect <node>`).

---

## 2. Quick Start

### Installation
```bash
git clone https://github.com/Syntra-Labs/tutor.git
cd tutor

# Build the native logical compiler (lgcc) and symlink CLI tools
make install
```

### CLI Workflow
```bash
# 1. Browse available problem sets
tutor list

# 2. Pick a problem and scaffold solution stub in your workspace (~/tutor-workspace/)
tutor pick 001-lockfree-ringbuffer

# 3. Run test harness against your solution
tutor test 001-lockfree-ringbuffer

# 4. Strip an existing source file into an empty problem stub
tutor slice src/engine.cpp -o stub.hpp

# 5. Pull new or updated problem sets and tests from remote
tutor pull

# 6. Connect to an optional distributed evaluation node
tutor connect cluster.syntralabs.org:9000
```

---

## 3. Neovim / Leetvim-Style Experience

Run `tutorvim` to open an isolated Neovim session loaded with the Tutor dashboard:
```bash
tutorvim
```

### Dashboard Keybindings:
- <kbd>Enter</kbd> or <kbd>p</kbd>: Pick the problem under cursor, scaffold files, and open `solution.<ext>`.
- <kbd>t</kbd>: Run test suite for the problem under cursor in a floating terminal.
- <kbd>r</kbd>: Refresh problem list and solved status.
- <kbd>q</kbd>: Close dashboard.

### User Commands (Available in Synvim & Neovim):
- `:Tutor` / `:Tutor list` — Open interactive problem dashboard.
- `:Tutor pick <id>` — Scaffold and open a specific problem.
- `:Tutor test` — Test current active problem solution.
- `:Tutor pull` — Pull updated problem sets and test harnesses.
- `:Tutor status` — View progress metrics and connected node.

---

## 4. Problem Catalog (Initial Seed Sets)

| ID | Title | Category | Difficulty | Focus |
| :--- | :--- | :--- | :--- | :--- |
| `001-lockfree-ringbuffer` | **SPSC Lock-Free Ring Buffer** | Concurrency & Systems | Medium | Atomic head/tail, `acquire`/`release` memory barriers |
| `002-hypervector-hamming` | **8192-bit Hypervector Hamming Distance** | Geometric Systems & Compute | Easy | SWAR / `std::popcount`, normalized cosine metric |
| `003-lru-cache-o1` | **O(1) Intrusive LRU Cache** | Data Structures & Memory | Medium | Hash index + linked list promotion & eviction |
| `004-ca184-demarcation` | **Rule 184 Boundary Demarcation** | Information Theory & Automata | Hard | 64-bit SWAR Rule 184 traffic particle conservation |

---

## 5. Logical Compiler & Problem Stub Generator (`lgcc`)

The native Rust logical compiler located in `crates/lgcc` is designed to analyze code for **logical errors, invariant breaches, and semantic contract bugs** (analogous to how `gcc` checks syntax and types, `lgcc` checks logic):
- **Logical Error & Bug Detection**: Analyzes implementations against declared preconditions, postconditions, and invariant contracts.
- **`lgcc --skeletonize <file>`**: Strips function bodies into empty problem stubs while keeping signatures and docstring specifications (`tutor slice <file>`).
- **`lgcc --extract <file>`**: Extracts AST symbols, call graphs, and structural types.

*(Note: Task DAG linting and task routing belong strictly to [`Syntra-Labs/taskflow`](https://github.com/Syntra-Labs/taskflow).)*

---

## 6. Directory Layout

```
tutor/
├── bin/
│   ├── tutor                  # Standalone CLI launcher
│   ├── tutorvim               # Neovim launcher (like leetvim)
│   └── lgcc                   # Compiled native Rust logical compiler
├── crates/
│   └── lgcc/                  # Logical compiler & problem stub generator
│       ├── src/
│       │   ├── ast/           # Extractor & function skeletonizer
│       │   ├── graph/         # Dependency graph & symbol table
│       │   ├── router/        # Context budgeting & prime router
│       │   └── spec/          # LexLean parser & task linter
│       └── Cargo.toml
├── lua/
│   └── tutor/
│       └── init.lua           # Neovim plugin (:Tutor, floating runner, dashboard)
├── problems/                  # Problem catalog
│   ├── 001-lockfree-ringbuffer/
│   ├── 002-hypervector-hamming/
│   ├── 003-lru-cache-o1/
│   └── 004-ca184-demarcation/
├── tutor/
│   ├── catalog.py             # Problem sets loader & indexer
│   ├── workspace.py           # Solution scaffolding engine
│   ├── runner.py              # Local compilation & test harness runner
│   ├── sync.py                # Remote pull & optional node connection
│   ├── state.py               # Local progress ledger (~/.local/share/tutor/)
│   └── cli.py                 # Colorized terminal interface
├── Makefile
├── pyproject.toml
└── README.md
```
