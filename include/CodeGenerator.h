#pragma once

#include "AST.h"
#include <string>
#include <sstream>

class CodeGenerator {
public:
    CodeGenerator() = default;

    // Entry point for code generation
    std::string generate(const ProgramNode* program);

private:
    int indentLevel = 0;
    std::ostringstream out;

    // Indentation helpers
    void indent();
    std::string getIndent() const;

    // Statement Generators
    void generateStatement(const StatementNode* stmt);
    void generateVarDecl(const VarDeclNode* decl);
    void generateAssign(const AssignStmtNode* assign);
    void generateIf(const IfStmtNode* ifStmt);
    void generateWhile(const WhileStmtNode* whileStmt);
    void generateBlock(const BlockStmtNode* block);
    void generatePrint(const PrintStmtNode* printStmt); // <-- THIS IS THE MISSING PIECE!

    // Expression Generators
    std::string generateExpression(const ExpressionNode* expr);
    std::string generateBinary(const BinaryExprNode* bin);
    std::string generateUnary(const UnaryExprNode* un);
    std::string generateNumber(const NumberLiteralNode* num);
    std::string generateBoolean(const BooleanLiteralNode* b);
    std::string generateVariable(const VariableExprNode* var);
};