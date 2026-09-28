// lexer.h - Phase 1: turns source text into tokens
#pragma once
#include <string>
#include <vector>

enum TokenType {
    T_INT_LIT, T_FLOAT_LIT, T_STRING, T_IDENT,
    // keywords
    T_PURNO, T_DOSHOMIK, T_JODI, T_NAHOLE, T_JOTOKKHON, T_DEKHAO,
    // operators
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_ASSIGN,
    T_EQ, T_NEQ, T_LT, T_GT, T_LE, T_GE,
    // punctuation
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_SEMI, T_COMMA,
    T_UNKNOWN, T_END
};

struct Token {
    TokenType type;
    std::string text;
    int line;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t i = 0;
    int line = 1;

    char peek(int offset = 0);
    Token make(TokenType type, const std::string& text);
    void skipSpacesAndComments();
    Token nextToken();
    Token readNumber();
    Token readWord();
    Token readString();
};
