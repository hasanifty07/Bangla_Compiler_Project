// lexer.cpp - Phase 1: source text -> tokens
#include "lexer.h"
#include <cctype>
using namespace std;

Lexer::Lexer(const string& source) : src(source) {}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (true) {
        Token t = nextToken();
        tokens.push_back(t);
        if (t.type == T_END) break;
    }
    return tokens;
}

char Lexer::peek(int offset) {
    return (i + offset < src.size()) ? src[i + offset] : '\0';
}

Token Lexer::make(TokenType type, const string& text) {
    return Token{type, text, line};
}

// skip spaces, new lines and # comments
void Lexer::skipSpacesAndComments() {
    while (i < src.size()) {
        char c = src[i];
        if (c == '\n')                       { line++; i++; }
        else if (isspace((unsigned char)c))  i++;
        else if (c == '#')                   { while (i < src.size() && src[i] != '\n') i++; }
        else break;
    }
}

Token Lexer::nextToken() {
    skipSpacesAndComments();
    if (i >= src.size()) return make(T_END, "end of file");

    char c = src[i];
    if (isdigit((unsigned char)c))             return readNumber();
    if (isalpha((unsigned char)c) || c == '_') return readWord();
    if (c == '"')                              return readString();

    i++;  // it is a symbol: consume it
    switch (c) {
        case '+': return make(T_PLUS,   "+");
        case '-': return make(T_MINUS,  "-");
        case '*': return make(T_STAR,   "*");
        case '/': return make(T_SLASH,  "/");
        case '(': return make(T_LPAREN, "(");
        case ')': return make(T_RPAREN, ")");
        case '{': return make(T_LBRACE, "{");
        case '}': return make(T_RBRACE, "}");
        case ';': return make(T_SEMI,   ";");
        case ',': return make(T_COMMA,  ",");
        case '=': if (peek() == '=') { i++; return make(T_EQ, "=="); }
                  return make(T_ASSIGN, "=");
        case '<': if (peek() == '=') { i++; return make(T_LE, "<="); }
                  return make(T_LT, "<");
        case '>': if (peek() == '=') { i++; return make(T_GE, ">="); }
                  return make(T_GT, ">");
        case '!': if (peek() == '=') { i++; return make(T_NEQ, "!="); }
                  break;
    }
    return make(T_UNKNOWN, string(1, c));
}

Token Lexer::readNumber() {
    string text;
    while (isdigit((unsigned char)peek())) text += src[i++];

    // decimal number like 3.14
    if (peek() == '.' && isdigit((unsigned char)peek(1))) {
        text += src[i++];  // the dot
        while (isdigit((unsigned char)peek())) text += src[i++];
        return make(T_FLOAT_LIT, text);
    }
    // Python does not allow 007, so remove leading zeros
    while (text.size() > 1 && text[0] == '0') text.erase(0, 1);
    return make(T_INT_LIT, text);
}

Token Lexer::readWord() {
    string text;
    while (isalnum((unsigned char)peek()) || peek() == '_') text += src[i++];

    if (text == "purno")     return make(T_PURNO, text);
    if (text == "doshomik")  return make(T_DOSHOMIK, text);
    if (text == "jodi")      return make(T_JODI, text);
    if (text == "nahole")    return make(T_NAHOLE, text);
    if (text == "jotokkhon") return make(T_JOTOKKHON, text);
    if (text == "dekhao")    return make(T_DEKHAO, text);
    return make(T_IDENT, text);
}

Token Lexer::readString() {
    i++;  // skip opening "
    string text;
    while (i < src.size() && src[i] != '"' && src[i] != '\n') text += src[i++];
    if (i >= src.size() || src[i] != '"')
        return make(T_UNKNOWN, "unterminated string");
    i++;  // skip closing "
    return make(T_STRING, text);
}
