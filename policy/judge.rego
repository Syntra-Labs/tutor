# METADATA
# title: Syntra Tutor Judge & Architecture Policies (OPA / Rego)
# description: >
#   This executable policy is the canonical, single source of truth for Syntra Tutor's
#   system invariants, threat models, submission boundaries, and deterministic benchmarking.
#   It serves simultaneously as the living architecture specification and the active CI gate.
# custom:
#   standard: CNCF Open Policy Agent (Rego)
#   philosophy: "Executable Policy as Code" (No passive markdown architecture docs)
package tutor.judge

import future.keywords.contains
import future.keywords.if
import future.keywords.in

default allow := false

# The submission PR or commit is permitted if and only if zero policy violations exist.
allow if {
    count(violation) == 0
}

# ==============================================================================
# POL-000: Principle of Executable Policy as Single Source of Truth
# Rationale: Passive architecture Markdown documents drift, rot, and cannot gate
# changes. Architecture rules must be declared in self-documenting Policy-as-Code
# that LLMs, developers, and CI pipelines execute directly.
# ==============================================================================

# ==============================================================================
# POL-001: Submitter Path Isolation & Zero Cross-Contamination
# Rationale: Submissions are strictly scoped to the author's own directory:
#   submissions/<actor>/<problem_id>/
# Competitor PRs must never touch any path outside their user namespace.
# Threat Model: User 'bob' attempting to overwrite or poison 'alice's solutions.
# ==============================================================================
violation contains msg if {
    some file in input.changed_files
    not is_actor_submission(file, input.actor)
    msg := sprintf("[POL-001 Path Isolation Breach]: File '%v' is outside submissions/%v/. Submissions are strictly sandboxed to the contributor's own directory.", [file, input.actor])
}

is_actor_submission(file, actor) if {
    startswith(file, concat("", ["submissions/", actor, "/"]))
}

# ==============================================================================
# POL-002: Allowed Artifact Whitelist (Purity Principle)
# Rationale: Only valid solution files (.hpp for C++ problems, .sh for Linux VM problems)
# are permitted inside a submission folder. Rogue binaries, dotfiles, or shell scripts
# disguised as headers are strictly rejected.
# ==============================================================================
violation contains msg if {
    some file in input.changed_files
    is_actor_submission(file, input.actor)
    not is_allowed_submission_file(file)
    msg := sprintf("[POL-002 Invalid Artifact]: File '%v' is not an allowed submission artifact. Submissions must strictly be 'solution.hpp' or 'solution.sh'.", [file])
}

is_allowed_submission_file(file) if {
    endswith(file, "/solution.hpp")
}

is_allowed_submission_file(file) if {
    endswith(file, "/solution.sh")
}

# ==============================================================================
# POL-003: Monolithic Leaderboard Direct Modification Ban (Zero-Conflict Append-Only)
# Rationale: To prevent merge conflicts and score spoofing, docs/data/leaderboard.json
# is append-only and aggregated exclusively by CI post-merge. PRs must never touch it.
# Threat Model: PR branches causing infinite git merge conflicts on a monolithic JSON.
# ==============================================================================
violation contains msg if {
    some file in input.changed_files
    file == "docs/data/leaderboard.json"
    msg := "[POL-003 Leaderboard Direct Edit]: 'docs/data/leaderboard.json' cannot be modified in a submission PR. Standings are recomputed post-merge by the automated Autolab pipeline."
}

# ==============================================================================
# POL-004: Locked Test Harness & Core Problem Catalog Immutability
# Rationale: Submission PRs cannot modify locked problem descriptions, harnesses,
# or verification check headers to bypass grading assertions.
# Threat Model: A student PR removing asserts in harness.cpp to achieve 100% score.
# ==============================================================================
violation contains msg if {
    some file in input.changed_files
    startswith(file, "problems/")
    msg := sprintf("[POL-004 Problem Specification Lock]: File '%v' belongs to the core problem catalog and cannot be modified in a competitor submission PR.", [file])
}

violation contains msg if {
    some file in input.changed_files
    startswith(file, ".github/")
    msg := sprintf("[POL-004 CI Infrastructure Lock]: File '%v' defines grading infrastructure and cannot be modified in an unprivileged submission PR.", [file])
}

# ==============================================================================
# POL-005: Deterministic Metrics Contract (Volatile Cloud Timing Ban)
# Rationale: Volatile cloud wall-clock timing (runtime_ms) suffers from timing
# jitter on virtualized CI runners. Scoring colspecs must specify deterministic
# primary metrics (cycles_per_op from cycle counters, code_bytes, or testcase count).
# ==============================================================================
violation contains msg if {
    some prob in input.catalog_problems
    colspec := object.get(prob, "colspec", [])
    count(colspec) > 0
    primary_metric := lower(colspec[0].hdr)
    is_wall_clock_metric(primary_metric)
    msg := sprintf("[POL-005 Non-Deterministic Metric]: Problem '%v' uses wall-clock metric '%v' as primary sort key. Deterministic cycle counters or test results are required.", [prob.id, primary_metric])
}

is_wall_clock_metric(name) if {
    contains(name, "wall")
}

is_wall_clock_metric(name) if {
    contains(name, "runtime_ms")
}

is_wall_clock_metric(name) if {
    contains(name, "elapsed")
}
