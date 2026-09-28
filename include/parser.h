// parser.h - Phase 2: checks the grammar and builds the AST
#pragma once
#include <string>
#include <vector>
#include "lexer.h"
#include "ast.h"

struct ParseError {};   // thrown on a syntax error, caught for recovery

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    NodePtr parse();
    int errors() const;

private:
    std::vector<Token> tokens;
    size_t pos = 0;        // which token we are looking at
    int errorCount = 0;
    int depth = 0;         // nesting depth of ( ) and { } (avoids stack overflow)
    int exprNodes = 0;     // size of the current expression (avoids stack overflow)

    // token helpers
    Token& current();
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& message);
    void error(const std::string& message, int line = -1);
    NodePtr newExprNode(NodeKind kind, const std::string& text, int line);

    // statements
    void parseStatementSafely(NodePtr parent);   // error recovery lives here
    NodePtr parseStatement();
    NodePtr declaration();
    NodePtr assignment();
    NodePtr ifStatement();
    NodePtr whileStatement();
    NodePtr printStatement();
    NodePtr block();
    NodePtr condition();
    bool isComparison(TokenType t);

    // expressions
    NodePtr expression();
    NodePtr term();
    NodePtr makeBinary(const Token& op, NodePtr left, NodePtr right);
    NodePtr factor();
};
