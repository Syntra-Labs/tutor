#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct CanonicalGoal {
    std::string canon_goal{};
    std::string canon_tactic{};
    std::unordered_map<std::string, std::string> sigma{}; // surface_ident -> canon_ident (v0, h0, p0)
};

inline const std::unordered_set<std::string>& lean_keywords() {
    static const std::unordered_set<std::string> kw = {
        "theorem", "lemma", "def", "example", "by", "where", "let", "have", "fun",
        "forall", "exists", "match", "with", "if", "then", "else", "do", "return",
        "show", "from", "suffices", "this", "using", "only", "at", "in",
        "Nat", "Int", "Real", "Bool", "Prop", "Type", "Sort", "List", "Set", "Fin",
        "True", "False", "And", "Or", "Not", "Iff", "Eq", "Ne", "LT", "LE", "GT", "GE",
        "rfl", "simp", "simpa", "simp_rw", "dsimp", "omega", "ring", "linarith", "decide",
        "aesop", "exact", "apply", "refine", "intro", "intros", "rintro", "revert", "clear",
        "cases", "rcases", "obtain", "induction", "rw", "erw", "nth_rw", "rewrite", "subst",
        "assumption", "contradiction", "constructor", "left", "right", "ext", "funext", "congr",
        "trivial", "sorry", "calc", "symm", "mp", "mpr", "fst", "snd", "refl", "trans"
    };
    return kw;
}

inline std::string to_turnstile_state(const std::string& goal_str) {
    // TODO: convert Lean 4 theorem declaration into unified `x : T\nh : P\n⊢ Target` format
    (void)goal_str;
    return "";
}

inline CanonicalGoal canonicalize_goal_and_tactic(
    const std::string& goal_str,
    const std::string& tactic_str = ""
) {
    // TODO: build alpha + premise map sigma (v0.., h0.., p0..) and rewrite goal + tactic
    (void)goal_str;
    (void)tactic_str;
    return {};
}

inline std::string invert_sigma(
    const std::string& canon_text,
    const std::unordered_map<std::string, std::string>& sigma
) {
    // TODO: apply inverse map (canon_ident -> surface_ident) to restore surface syntax
    (void)canon_text;
    (void)sigma;
    return "";
}
