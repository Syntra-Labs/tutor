# METADATA
# title: Syntra Tutor Architecture-as-Code & System Invariants (OPA / Rego)
# description: >
#   Canonical declarative model of the Syntra Tutor platform architecture,
#   system components, data flows, and active invariant enforcement rules.
#   Enforces the "Architecture as Code" principle: all system specifications
#   must be executable code, explicitly prohibiting passive Markdown documents.
# custom:
#   standard: CNCF Open Policy Agent (Rego)
#   paradigm: Architecture-as-Code (AaC)
package tutor.architecture

import future.keywords.contains
import future.keywords.if
import future.keywords.in

default allow := false

# The codebase satisfies architecture invariants if and only if there are zero violations.
allow if {
    count(violations) == 0
}

# ==============================================================================
# 1. SYSTEM TOPOLOGY & ARCHITECTURE MODEL
# ==============================================================================

# Declared system components and boundaries
components := {
    "web_studio": {
        "tier": "frontend",
        "runtime": "Browser (WebAssembly)",
        "tech": ["Neovim WASM (Asyncify)", "v86 Linux 5.6 WASM", "Compiler Explorer API"],
        "purpose": "Zero-install client-side editing, local test execution, and 1-click PR creation."
    },
    "cli_engine": {
        "tier": "cli_and_local_server",
        "runtime": "Host Python 3.10+ / Linux",
        "tech": ["http.server", "g++ -std=c++20", "subprocess sandbox"],
        "purpose": "Local sub-50ms judging, DAG progression queries, and development workflows."
    },
    "catalog_dag": {
        "tier": "data_contract",
        "runtime": "Static Catalog",
        "tech": ["JSON Schema", "Prerequisite DAG (Directed Acyclic Graph)"],
        "purpose": "Defines 76 core problems, dependency edges, starter stubs, and locked harnesses."
    },
    "leaderboard_aggregator": {
        "tier": "backend_ci",
        "runtime": "GitHub Actions Post-Merge Workflow",
        "tech": ["Python Autolab Reducer", "colspec vector sorting"],
        "purpose": "Aggregates atomic verified submission files into docs/data/leaderboard.json without merge conflicts."
    },
    "policy_engine": {
        "tier": "governance",
        "runtime": "OPA Rego",
        "tech": ["Open Policy Agent", "Architecture-as-Code"],
        "purpose": "Executable enforcement of architecture boundaries, path sandboxing, and doc bans."
    },
    "issue_tracker_dag": {
        "tier": "distributed_governance",
        "runtime": "GitHub Issues & Projects API",
        "tech": ["Parent-Epic / Child-Spike Hierarchy", "Upstream Channel Links", "Bidirectional Tasklists"],
        "purpose": "Decentralized tracking of branching architectural concerns, upstream repository channels, and verifiable decision gates."
    }
}

# Declared data stores and ownership
datastores := {
    "problem_catalog": {
        "path": "problems/<id>/",
        "access": "read_only_for_competitors",
        "schema": "problem.json (id, track, category, depends_on, colspec, harness, stub)"
    },
    "user_submissions": {
        "path": "submissions/<user>/<id>/",
        "access": "contributor_isolated",
        "artifacts": ["solution.hpp", "solution.sh"]
    },
    "static_pages_cache": {
        "path": "docs/data/",
        "access": "ci_aggregated_only",
        "files": ["catalog.json", "leaderboard.json"]
    }
}

# ==============================================================================
# 2. ACTIVE ARCHITECTURAL INVARIANTS & VIOLATION RULES
# ==============================================================================

# ------------------------------------------------------------------------------
# ARCH-001: Zero Markdown Invariant ("No Passive Documentation in Codebase")
# Rationale: Passive Markdown files drift, rot, and cannot actively validate state.
# All documentation, roadmaps, and audits must be converted into executable code,
# structured JSON schemas, or tracked GitHub Issues/Milestones.
# The root README.md is the sole permitted entrypoint for repository discovery.
# ------------------------------------------------------------------------------
violations contains msg if {
    some file in input.all_files
    endswith(file, ".md")
    not is_root_manifest(file)
    target := categorize_markdown_migration(file)
    msg := sprintf("[ARCH-001 Markdown Prohibition]: Passive file '%v' is forbidden. Migration target: %v.", [file, target])
}

is_root_manifest(file) if file == "README.md"
is_root_manifest(file) if file == "./README.md"

# Classifies existing legacy markdown files into their assigned migration destinations
categorize_markdown_migration(file) := "Convert to structured policy or GitHub Project Milestone" if {
    file == "./ROADMAP_MATH_AND_ML.md"
} else := "Convert to GitHub Architecture Issue" if {
    file == "./docs/PROBLEM_MANAGEMENT_SCALING_AUDIT.md"
} else := "Embed 'spec' text directly inside problem.json ('description' schema field)" if {
    contains(file, "/problems/")
} else := "Convert to minimal repo manifest or GitHub About description" if {
    file == "./README.md"
} else := "Convert to structured JSON/Rego policy or GitHub Issue"

# ------------------------------------------------------------------------------
# ARCH-002: Submitter Namespace Isolation
# Submissions by actor @user can strictly modify files under submissions/<actor>/.
# ------------------------------------------------------------------------------
violations contains msg if {
    some file in input.changed_files
    not startswith(file, concat("", ["submissions/", input.actor, "/"]))
    msg := sprintf("[ARCH-002 Namespace Breach]: Contributor @%v modified '%v' outside their isolated directory 'submissions/%v/'.", [input.actor, file, input.actor])
}

# ------------------------------------------------------------------------------
# ARCH-003: Core Catalog Immutability from Submission PRs
# Submission PRs cannot tamper with problems or CI infrastructure.
# ------------------------------------------------------------------------------
violations contains msg if {
    some file in input.changed_files
    startswith(file, "problems/")
    msg := sprintf("[ARCH-003 Catalog Tamper]: Core problem catalog '%v' is immutable in submission PRs.", [file])
}

violations contains msg if {
    some file in input.changed_files
    startswith(file, ".github/")
    msg := sprintf("[ARCH-003 Infrastructure Tamper]: CI workflow '%v' is immutable in submission PRs.", [file])
}

# ------------------------------------------------------------------------------
# ARCH-004: Monolithic Leaderboard Modification Ban (Append-Only Aggregation)
# Direct commits to docs/data/leaderboard.json in PRs are blocked.
# ------------------------------------------------------------------------------
violations contains msg if {
    some file in input.changed_files
    file == "docs/data/leaderboard.json"
    msg := "[ARCH-004 Leaderboard Direct Edit]: 'docs/data/leaderboard.json' cannot be edited directly. The board is aggregated post-merge."
}

# ------------------------------------------------------------------------------
# ARCH-005: Deterministic Benchmark Contract (Volatile Wall-Clock Ban)
# Scoring colspecs must specify deterministic primary metrics (cycles_per_op, code_bytes).
# ------------------------------------------------------------------------------
violations contains msg if {
    some prob in input.catalog_problems
    colspec := object.get(prob, "colspec", [])
    count(colspec) > 0
    hdr := lower(colspec[0].hdr)
    is_volatile_metric(hdr)
    msg := sprintf("[ARCH-005 Volatile Timing Ban]: Problem '%v' uses non-deterministic metric '%v'. Hardware cycles or testcase pass rate required.", [prob.id, hdr])
}

is_volatile_metric(name) if contains(name, "wall")
is_volatile_metric(name) if contains(name, "runtime_ms")
is_volatile_metric(name) if contains(name, "elapsed")

# ------------------------------------------------------------------------------
# ARCH-006: Catalog Schema Completeness & Colspec Registration
# Every catalog problem must declare complete metadata: id, title, track,
# difficulty, colspec, and supported languages.
# ------------------------------------------------------------------------------
violations contains msg if {
    some prob in input.catalog_problems
    not prob.id
    msg := "[ARCH-006 Incomplete Metadata]: Problem definition missing mandatory 'id' field."
}

violations contains msg if {
    some prob in input.catalog_problems
    not prob.colspec
    msg := sprintf("[ARCH-006 Missing Colspec]: Problem '%v' omits 'colspec' scoring vector definition.", [prob.id])
}

violations contains msg if {
    some prob in input.catalog_problems
    not prob.languages
    msg := sprintf("[ARCH-006 Missing Languages]: Problem '%v' omits 'languages' whitelist.", [prob.id])
}

# ------------------------------------------------------------------------------
# ARCH-007: Prerequisite Skill-Tree DAG Reference Integrity
# Every dependency referenced in depends_on must exist in the problem catalog.
# ------------------------------------------------------------------------------
violations contains msg if {
    some prob in input.catalog_problems
    some dep in object.get(prob, "depends_on", [])
    not catalog_has_id(dep, input.catalog_problems)
    msg := sprintf("[ARCH-007 Broken DAG Edge]: Problem '%v' references nonexistent prerequisite '%v'.", [prob.id, dep])
}

catalog_has_id(id, problems) if {
    some p in problems
    p.id == id
}

# ------------------------------------------------------------------------------
# ARCH-008: Distributed Issue-Tracking & Upstream Channel Federation Protocol
# Rationale: Complex architectural explorations, cross-system integrations
# (e.g. UOR/Kappa), and branching concerns must not rot in untracked notes.
# Invariants:
# 1. Epic-Spike Hierarchy: Every major research exploration must anchor to a
#    Parent Tracking Epic decomposing orthogonal concerns into focused Child Spikes.
# 2. Upstream Linkage: Any external investigation targeting an upstream repo
#    must maintain a formal upstream connection channel with bidirectional links.
# 3. Explicit Blocking Gates: Spikes must declare their blocking relationships
#    to downstream milestone decisions.
# ------------------------------------------------------------------------------
issue_tracking_protocol := {
    "standard": "Federated GitHub Issues & Bidirectional Linking",
    "hierarchy": "Parent Epic -> Child Spikes",
    "upstream_channels": "Cross-repo issue referencing (e.g. UOR-Foundation/kappa-registry)",
    "lifecycle": ["Triage", "Spike Investigation", "Upstream Channel Link", "Decision Gate", "Close"]
}
