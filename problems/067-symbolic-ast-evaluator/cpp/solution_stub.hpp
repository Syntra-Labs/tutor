#pragma once

#include <cmath>
#include <cstddef>
#include <string>

namespace sym {

struct Tree {
    Tree* left = nullptr;
    std::string data{};
    Tree* right = nullptr;

    Tree() = default;
    Tree(Tree* l, std::string d, Tree* r) : left(l), data(std::move(d)), right(r) {}
};

inline void deallocateTree(Tree& root) {
    if (root.left) {
        deallocateTree(*root.left);
        delete root.left;
        root.left = nullptr;
    }
    if (root.right) {
        deallocateTree(*root.right);
        delete root.right;
        root.right = nullptr;
    }
}

inline void parseExpression(Tree& expression, int& p, const std::string& str) {
    // TODO: implement recursive-descent parser for +, -, *, /, ^, and parentheses (...)
    (void)expression;
    (void)p;
    (void)str;
}

inline double evalExpression(const Tree* expression) {
    // TODO: recursively evaluate binary expression AST
    (void)expression;
    return 0.0;
}

inline double parse_and_eval(const std::string& expr_str) {
    Tree root;
    int p = 0;
    parseExpression(root, p, expr_str);
    double val = evalExpression(&root);
    deallocateTree(root);
    return val;
}

} // namespace sym
