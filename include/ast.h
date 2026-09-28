// ast.h - the Abstract Syntax Tree (the tree the parser builds)
#pragma once
#include <string>
#include <vector>
#include <memory>

enum Type { TYPE_INT, TYPE_FLOAT };   // purno, doshomik

enum NodeKind {
    N_PROGRAM,   // children: statements
    N_BLOCK,     // children: statements
    N_DECLARE,   // text = variable name, type = declared type, children: [initial value]
    N_ASSIGN,    // text = variable name, children: [value]
    N_IF,        // children: [condition, then-block, else-block (optional)]
    N_WHILE,     // children: [condition, block]
    N_PRINT,     // children: items (N_STRING or expressions)
    N_COMPARE,   // text = operator, children: [left, right]
    N_BINARY,    // text = + - * /, children: [left, right]
    N_NEGATE,    // children: [operand]
    N_INT, N_FLOAT, N_VAR, N_STRING   // leaves, text = the value / name
};

struct Node;
typedef std::shared_ptr<Node> NodePtr;

struct Node {
    NodeKind kind;
    std::string text;
    int line;
    Type type = TYPE_INT;          // filled in by the SemanticAnalyzer
    std::vector<NodePtr> children;

    Node(NodeKind k, const std::string& t, int l) : kind(k), text(t), line(l) {}
};

inline NodePtr makeNode(NodeKind kind, const std::string& text, int line) {
    return std::make_shared<Node>(kind, text, line);
}
