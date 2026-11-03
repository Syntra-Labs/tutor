#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class ExprKind : uint8_t {
    BVAR = 0,
    FVAR = 1,
    MVAR = 2,
    SORT = 3,
    CONST = 4,
    APP = 5,
    LAM = 6,
    FORALLE = 7,
    LETE = 8,
    LIT = 9,
    PROJ = 10
};

struct ExprNode {
    ExprKind k = ExprKind::BVAR;
    std::string label{};
    std::vector<int> children{};
};

struct Hyp {
    std::string fv{};
    int type = 0;
    bool is_prop = true;
};

struct Move {
    std::string kind{};   // "exact", "intro", "apply", "rw"
    std::string source{}; // fv or lemma name ("" for intro)
    bool reverse = false; // true for rw <-

    bool operator==(const Move& o) const {
        return kind == o.kind && source == o.source && reverse == o.reverse;
    }
};

class HashConsDAG {
public:
    int intern(ExprKind k, const std::string& label, const std::vector<int>& children = {}) {
        // TODO: deduplicate (k, label, children) and return canonical node index in nodes_
        (void)k;
        (void)label;
        (void)children;
        return 0;
    }

    int mk_app(int fn, int arg) {
        return intern(ExprKind::APP, "", {fn, arg});
    }

    int mk_eq(int type_node, int lhs, int rhs) {
        int eq_c = intern(ExprKind::CONST, "Eq", {});
        return mk_app(mk_app(mk_app(eq_c, type_node), lhs), rhs);
    }

    const std::vector<ExprNode>& nodes() const { return nodes_; }

    int head(int n) const {
        while (nodes_[n].k == ExprKind::APP && !nodes_[n].children.empty()) {
            n = nodes_[n].children[0];
        }
        return n;
    }

    std::vector<int> subterms(int root) const {
        // TODO: collect deduplicated reachable subterm IDs in pre-order
        (void)root;
        return {};
    }

private:
    std::vector<ExprNode> nodes_;
};

inline bool match_expr(
    const HashConsDAG& dag,
    int pattern,
    int target,
    int depth,
    int local,
    std::unordered_map<int, int>& bindings
) {
    // TODO: implement first-order DAG pattern unification with de Bruijn wildcards
    (void)dag;
    (void)pattern;
    (void)target;
    (void)depth;
    (void)local;
    (void)bindings;
    return false;
}

inline std::vector<Move> admissible_moves(
    const HashConsDAG& dag,
    const std::vector<Hyp>& hyps,
    int target,
    const std::vector<std::pair<std::string, int>>& lemmas
) {
    // TODO: filter admissible exact, intro, apply, and rw moves over the hash-consed DAG
    (void)dag;
    (void)hyps;
    (void)target;
    (void)lemmas;
    return {};
}
