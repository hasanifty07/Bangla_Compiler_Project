#pragma once

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <string>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // Entry point for parsing
    std::unique_ptr<ProgramNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Statement Parsing (BNF: <statement>)
    std::unique_ptr<StatementNode> parseStatement();
    std::unique_ptr<VarDeclNode> parseDeclaration();
    std::unique_ptr<AssignStmtNode> parseAssignment();
    std::unique_ptr<IfStmtNode> parseIfStatement();
    std::unique_ptr<WhileStmtNode> parseWhileStatement();
    std::unique_ptr<BlockStmtNode> parseBlock();
    std::unique_ptr<PrintStmtNode> parsePrintStatement();

    // Expression & Condition Parsing (Precedence Hierarchy)
    std::unique_ptr<ExpressionNode> parseCondition();
    std::unique_ptr<ExpressionNode> parseExpression(); // Level 1: +, -
    std::unique_ptr<ExpressionNode> parseTerm();       // Level 2: *, /
    std::unique_ptr<ExpressionNode> parseFactor();     // Level 3: Unary, Grouping, Literals, IDs

    // Token Stream Navigation & Utilities
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& errorMessage);

    void error(const Token& token, const std::string& message) const;
    static long long parseBanglaOrAsciiInteger(const std::string& lexeme);
};