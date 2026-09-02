#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// ==========================================
// Base AST Node
// ==========================================
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// ==========================================
// Statement Base & Derived Nodes
// ==========================================
class StatementNode : public ASTNode {};

class ExpressionNode : public ASTNode {};

// Represents the entire program root
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<StatementNode>> statements;

    void print(int indent = 0) const override;
};

// Block Statement: { statement_list }
class BlockStmtNode : public StatementNode {
public:
    std::vector<std::unique_ptr<StatementNode>> statements;

    void print(int indent = 0) const override;
};

// Variable Declaration: ধরি <type> <id> [= <expr>];
class VarDeclNode : public StatementNode {
public:
    std::string typeName;       // "সংখ্যা" or "যৌক্তিক"
    std::string identifier;
    std::unique_ptr<ExpressionNode> initializer; // nullptr if uninitialized

    VarDeclNode(std::string typeName, std::string identifier, std::unique_ptr<ExpressionNode> initializer)
        : typeName(std::move(typeName)), identifier(std::move(identifier)), initializer(std::move(initializer)) {}

    void print(int indent = 0) const override;
};

// Assignment Statement: <id> = <expr>;
class AssignStmtNode : public StatementNode {
public:
    std::string identifier;
    std::unique_ptr<ExpressionNode> value;

    AssignStmtNode(std::string identifier, std::unique_ptr<ExpressionNode> value)
        : identifier(std::move(identifier)), value(std::move(value)) {}

    void print(int indent = 0) const override;
};

// If-Else Statement: যদি (<condition>) <block> [নাহলে <block>]
class IfStmtNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockStmtNode> thenBranch;
    std::unique_ptr<BlockStmtNode> elseBranch; // nullptr if no else block

    IfStmtNode(std::unique_ptr<ExpressionNode> condition,
               std::unique_ptr<BlockStmtNode> thenBranch,
               std::unique_ptr<BlockStmtNode> elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void print(int indent = 0) const override;
};

// While Loop: যতক্ষণ (<condition>) <block>
class WhileStmtNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<BlockStmtNode> body;

    WhileStmtNode(std::unique_ptr<ExpressionNode> condition, std::unique_ptr<BlockStmtNode> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void print(int indent = 0) const override;
};

// ==========================================
// Expression Derived Nodes
// ==========================================

// Binary Operations (+, -, *, /, ==, !=, <, <=, >, >=)
class BinaryExprNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    std::string op;
    std::unique_ptr<ExpressionNode> right;

    BinaryExprNode(std::unique_ptr<ExpressionNode> left, std::string op, std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    void print(int indent = 0) const override;
};

// Unary Operations (-factor)
class UnaryExprNode : public ExpressionNode {
public:
    std::string op;
    std::unique_ptr<ExpressionNode> operand;

    UnaryExprNode(std::string op, std::unique_ptr<ExpressionNode> operand)
        : op(std::move(op)), operand(std::move(operand)) {}

    void print(int indent = 0) const override;
};

// Integer Literals
class NumberLiteralNode : public ExpressionNode {
public:
    std::string rawLexeme;
    long long value;

    NumberLiteralNode(std::string rawLexeme, long long value)
        : rawLexeme(std::move(rawLexeme)), value(value) {}

    void print(int indent = 0) const override;
};

// Boolean Literals (সত্য / মিথ্যা)
class BooleanLiteralNode : public ExpressionNode {
public:
    bool value;
    std::string rawLexeme;

    BooleanLiteralNode(bool value, std::string rawLexeme)
        : value(value), rawLexeme(std::move(rawLexeme)) {}

    void print(int indent = 0) const override;
};

// Identifier / Variable Read
class VariableExprNode : public ExpressionNode {
public:
    std::string name;

    explicit VariableExprNode(std::string name) : name(std::move(name)) {}

    void print(int indent = 0) const override;
};
// Print Statement: লেখো <expr>;
class PrintStmtNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> expression;

    explicit PrintStmtNode(std::unique_ptr<ExpressionNode> expression)
        : expression(std::move(expression)) {}

    void print(int indent = 0) const override;
};