// parser.cpp - Phase 2: tokens -> AST (grammar only)
#include "parser.h"
#include <iostream>
using namespace std;

Parser::Parser(const vector<Token>& t) : tokens(t) {}

NodePtr Parser::parse() {
    NodePtr program = makeNode(N_PROGRAM, "program", 1);
    while (current().type != T_END) parseStatementSafely(program);
    return program;
}

int Parser::errors() const { return errorCount; }

// ---------- token helpers ----------
Token& Parser::current() { return tokens[pos]; }

Token Parser::advance() {
    Token t = tokens[pos];
    if (pos < tokens.size() - 1) pos++;   // never move past T_END
    return t;
}

bool Parser::match(TokenType type) {      // consume if it matches
    if (current().type == type) { advance(); return true; }
    return false;
}

Token Parser::expect(TokenType type, const string& message) {
    if (current().type != type) {
        // a missing ';' belongs to the end of the previous line
        int line = (type == T_SEMI && pos > 0) ? tokens[pos - 1].line
                                               : current().line;
        error(message + " but found '" + current().text + "'", line);
    }
    return advance();
}

void Parser::error(const string& message, int line) {
    if (line < 0) line = current().line;
    cout << "Line " << line << ": syntax error: " << message << endl;
    errorCount++;
    throw ParseError();
}

NodePtr Parser::newExprNode(NodeKind kind, const string& text, int line) {
    if (++exprNodes > 1000) error("expression is too large");
    return makeNode(kind, text, line);
}

// ---------- error recovery ----------
// Parse one statement and add it to 'parent'.
// If it fails, skip to the next ';' and continue.
void Parser::parseStatementSafely(NodePtr parent) {
    size_t start = pos;
    int savedDepth = depth;
    exprNodes = 0;
    try {
        parent->children.push_back(parseStatement());
    } catch (ParseError&) {
        depth = savedDepth;
        while (current().type != T_END) {
            if (current().type == T_SEMI)   { advance(); break; }
            if (current().type == T_RBRACE) break;
            advance();
        }
        // make sure we always move forward (no infinite loop)
        if (pos == start && current().type != T_END) advance();
    }
}

// ---------- statements ----------
NodePtr Parser::parseStatement() {
    switch (current().type) {
        case T_PURNO:
        case T_DOSHOMIK:  return declaration();
        case T_IDENT:     return assignment();
        case T_JODI:      return ifStatement();
        case T_JOTOKKHON: return whileStatement();
        case T_DEKHAO:    return printStatement();
        default: error("unexpected '" + current().text + "'");
    }
    return nullptr;  // never reached
}

// purno x = 5;      doshomik y;
NodePtr Parser::declaration() {
    Token keyword = advance();
    Token name = expect(T_IDENT, "variable name expected");
    NodePtr node = makeNode(N_DECLARE, name.text, name.line);
    node->type = (keyword.type == T_PURNO) ? TYPE_INT : TYPE_FLOAT;
    if (match(T_ASSIGN)) node->children.push_back(expression());
    expect(T_SEMI, "';' expected");
    return node;
}

// x = x + 1;
NodePtr Parser::assignment() {
    Token name = advance();
    expect(T_ASSIGN, "'=' expected");
    NodePtr node = makeNode(N_ASSIGN, name.text, name.line);
    node->children.push_back(expression());
    expect(T_SEMI, "';' expected");
    return node;
}

// jodi (x > 5) { ... } nahole { ... }
NodePtr Parser::ifStatement() {
    Token keyword = advance();
    NodePtr node = makeNode(N_IF, "jodi", keyword.line);
    node->children.push_back(condition());
    node->children.push_back(block());
    if (match(T_NAHOLE)) node->children.push_back(block());
    return node;
}

// jotokkhon (x < 10) { ... }
NodePtr Parser::whileStatement() {
    Token keyword = advance();
    NodePtr node = makeNode(N_WHILE, "jotokkhon", keyword.line);
    node->children.push_back(condition());
    node->children.push_back(block());
    return node;
}

// dekhao("Hello", x);
NodePtr Parser::printStatement() {
    Token keyword = advance();
    NodePtr node = makeNode(N_PRINT, "dekhao", keyword.line);
    expect(T_LPAREN, "'(' expected");
    do {
        if (current().type == T_STRING) {
            Token s = advance();
            node->children.push_back(makeNode(N_STRING, s.text, s.line));
        } else {
            node->children.push_back(expression());
        }
    } while (match(T_COMMA));
    expect(T_RPAREN, "')' expected");
    expect(T_SEMI, "';' expected");
    return node;
}

// { statement statement ... }
NodePtr Parser::block() {
    Token open = expect(T_LBRACE, "'{' expected");
    if (++depth > 200) error("blocks are nested too deeply");
    NodePtr node = makeNode(N_BLOCK, "block", open.line);
    while (current().type != T_RBRACE && current().type != T_END)
        parseStatementSafely(node);
    expect(T_RBRACE, "'}' expected");
    depth--;
    return node;
}

// ( expression  <comparison>  expression )
NodePtr Parser::condition() {
    expect(T_LPAREN, "'(' expected");
    NodePtr left = expression();
    if (!isComparison(current().type))
        error("comparison operator (< > <= >= == !=) expected but found '"
              + current().text + "'");
    Token op = advance();
    NodePtr right = expression();
    expect(T_RPAREN, "')' expected");

    NodePtr node = makeNode(N_COMPARE, op.text, op.line);
    node->children.push_back(left);
    node->children.push_back(right);
    return node;
}

bool Parser::isComparison(TokenType t) {
    return t == T_LT || t == T_GT || t == T_LE ||
           t == T_GE || t == T_EQ || t == T_NEQ;
}

// ---------- expressions (precedence: * / before + -) ----------
// expression = term { (+|-) term }
NodePtr Parser::expression() {
    NodePtr left = term();
    while (current().type == T_PLUS || current().type == T_MINUS) {
        Token op = advance();
        NodePtr right = term();
        left = makeBinary(op, left, right);
    }
    return left;
}

// term = factor { (*|/) factor }
NodePtr Parser::term() {
    NodePtr left = factor();
    while (current().type == T_STAR || current().type == T_SLASH) {
        Token op = advance();
        NodePtr right = factor();
        left = makeBinary(op, left, right);
    }
    return left;
}

NodePtr Parser::makeBinary(const Token& op, NodePtr left, NodePtr right) {
    NodePtr node = newExprNode(N_BINARY, op.text, op.line);
    node->children.push_back(left);
    node->children.push_back(right);
    return node;
}

// factor = number | variable | ( expression ) | - factor
NodePtr Parser::factor() {
    if (++depth > 200) error("expression is nested too deeply");
    Token t = current();
    NodePtr result;

    if (t.type == T_INT_LIT) {
        advance();
        result = newExprNode(N_INT, t.text, t.line);
    } else if (t.type == T_FLOAT_LIT) {
        advance();
        result = newExprNode(N_FLOAT, t.text, t.line);
    } else if (t.type == T_IDENT) {
        advance();
        result = newExprNode(N_VAR, t.text, t.line);
    } else if (t.type == T_LPAREN) {
        advance();
        result = expression();
        expect(T_RPAREN, "')' expected");
    } else if (t.type == T_MINUS) {
        advance();
        NodePtr inner = factor();
        result = newExprNode(N_NEGATE, "-", t.line);
        result->children.push_back(inner);
    } else {
        error("expression expected but found '" + t.text + "'");
    }
    depth--;
    return result;
}
