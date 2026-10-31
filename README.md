# tutor

> **"A Coding Tutor Rather Than a Coding Agent."**  
> Syntra Labs Cognitive Coding Tutor, Socratic Verification Arena & Systems Problem Substrate.

---

## 1. Executive Vision

The modern software landscape is inundated with autonomous "coding agents" designed to write code *for* the engineer. While effective for trivial tasks, this paradigm introduces cognitive atrophy, reduces architectural agency, and creates verification bottlenecks where engineers review code they neither derived nor deeply understand.

**Syntra Labs rejects passive generation in favor of active synthesis.**

`tutor` is built to empower human mastery and systems engineering through:
- **Socratic Tutoring, Not Autonomous Generation**: Acts as an adversarial interlocutor and dialectic evaluator that guides learners to discover structural invariants, systems-level trade-offs, and algorithmic proofs.
- **Clean-Room Production Slicing**: Hides existing implementations behind opaque contract barriers. Users see only formal type signatures, Hoare specifications (`requires` / `ensures`), and property harnesses.
- **Distributed Systems Challenge Arena**: Ephemeral multi-node container environments with automated fault injection (packet drops, latency jitter, split-brain partitions) and Jepsen-style linearizability tests.
- **Formal Verification Integration**: Native Lean 4 theorem proving, automated Z3 SMT contract checking, and Hoare-logic invariant validation.
- **Adaptive Frontier Routing**: Models the engineer's skill profile across algebraic reasoning, concurrency, memory efficiency, and invariants, calculating the Zone of Proximal Development (ZPD).

---

## 2. Core Architectural Pillars

```
                              ┌───────────────────────────────────┐
                              │     Learner / Systems Engineer    │
                              └─────────────────┬─────────────────┘
                                                │ Progress, Invariant Derivations
                                                ▼
                              ┌───────────────────────────────────┐
                              │      Adaptive Cognitive Router    │
                              └─────────────────┬─────────────────┘
                                                │ Routed Challenge Specification
                    ┌───────────────────────────┴───────────────────────────┐
                    ▼                                                       ▼
  ┌───────────────────────────────────┐                   ┌───────────────────────────────────┐
  │  Clean-Room Production Slice      │                   │ Distributed Systems Arena         │
  │  • Opaque contract signature      │                   │ • Multi-container topology        │
  │  • Strict Hoare specification     │                   │ • Fault injection & Jepsen tests  │
  │  • No exposure to production code │                   │ • Ephemeral cluster management    │
  └─────────────────┬─────────────────┘                   └─────────────────┬─────────────────┘
                    │                                                       │
                    └───────────────────────────┬───────────────────────────┘
                                                ▼
                              ┌───────────────────────────────────┐
                              │ Distributed Verification Pipeline │
                              │ • Lean 4 Formal Verification Gate │
                              │ • Property-Based Fuzzing Engine   │
                              │ • Comparative Cycle Profiling     │
                              └───────────────────────────────────┘
```

---

## 3. Technology Stack

| Subsystem | Responsibility | Core Technologies |
| :--- | :--- | :--- |
| **Cognitive Core** | Socratic dialogue, conceptual hints, AST invariant checks | Lean 4, Dialectic Evaluators, Tree-sitter |
| **Contract Slicer** | Extracts isolated function boundaries from real-world repos | Tree-sitter, Clang LibTooling, `cargo-expand` |
| **Verification Engine** | Hoare contract validation, property fuzzing, formal proofs | Lean 4, Z3 SMT, AFL++, QuickCheck |
| **Arena Orchestrator** | Multi-node challenge sandboxes, chaos injection | MicroVMs, Podman/Docker, Linux cgroups |
| **Cognitive Router** | Skill tracing and Zone of Proximal Development dispatch | Manifold hypervectors, Bayesian Knowledge Tracing |

---

## 4. Status & Roadmap

- [ ] **Phase 1**: Clean-Room Contract Slicing & Verification Harness
- [ ] **Phase 2**: Distributed Cluster Challenge Arena & Chaos Controller
- [ ] **Phase 3**: Lean 4 / Hoare-logic Dual-Challenge Engine
- [ ] **Phase 4**: Automated Upstream PR Promotion & Skill Graph Router

---

## 5. Ecosystem

Part of the **Syntra Labs** systems ecosystem:
- [`router`](https://github.com/Syntra-Labs/router) — High-throughput geometric substrate & hypervector router
- [`synvim`](https://github.com/Syntra-Labs/proto-synvim) — Sovereign development environment
