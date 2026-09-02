#pragma once

#include "Token.h"
#include <string>
#include <unordered_map>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string source);

    Token getNextToken();
    std::vector<Token> tokenizeAll();

private:
    std::string source;
    size_t cursor = 0;
    size_t line = 1;
    size_t column = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token readIdentifierOrKeyword();
    Token readNumber();

    // UTF-8 helper functions
    static size_t getUtf8ByteLength(unsigned char leadByte);
    static bool isBanglaDigit(const std::string& utf8Char);
    static bool isBanglaLetterOrMark(unsigned char leadByte);
};