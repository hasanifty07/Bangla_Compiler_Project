#pragma once

#include <string>

enum class TokenType {
    // Keywords
    KEYWORD_DHORI,
    KEYWORD_SONGKHA,
    KEYWORD_JOUKTIK,
    KEYWORD_JODI,
    KEYWORD_NAHOLE,
    KEYWORD_JOTOKKHON,
    KEYWORD_LEKHO,      // <-- The new print command

    // Literals
    LITERAL_SHOTTO,
    LITERAL_MITTHA,
    IDENTIFIER,
    NUMBER,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    ASSIGN,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    // Punctuation
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
};

inline const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD_DHORI:     return "KEYWORD_DHORI";
        case TokenType::KEYWORD_SONGKHA:   return "KEYWORD_SONGKHA";
        case TokenType::KEYWORD_JOUKTIK:   return "KEYWORD_JOUKTIK";
        case TokenType::KEYWORD_JODI:      return "KEYWORD_JODI";
        case TokenType::KEYWORD_NAHOLE:    return "KEYWORD_NAHOLE";
        case TokenType::KEYWORD_JOTOKKHON: return "KEYWORD_JOTOKKHON";
        case TokenType::KEYWORD_LEKHO:     return "KEYWORD_LEKHO";
        case TokenType::LITERAL_SHOTTO:    return "LITERAL_SHOTTO";
        case TokenType::LITERAL_MITTHA:    return "LITERAL_MITTHA";
        case TokenType::IDENTIFIER:        return "IDENTIFIER";
        case TokenType::NUMBER:            return "NUMBER";
        case TokenType::PLUS:              return "PLUS";
        case TokenType::MINUS:             return "MINUS";
        case TokenType::STAR:              return "STAR";
        case TokenType::SLASH:             return "SLASH";
        case TokenType::ASSIGN:            return "ASSIGN";
        case TokenType::EQUAL_EQUAL:       return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL:         return "NOT_EQUAL";
        case TokenType::LESS:              return "LESS";
        case TokenType::LESS_EQUAL:        return "LESS_EQUAL";
        case TokenType::GREATER:           return "GREATER";
        case TokenType::GREATER_EQUAL:     return "GREATER_EQUAL";
        case TokenType::LPAREN:            return "LPAREN";
        case TokenType::RPAREN:            return "RPAREN";
        case TokenType::LBRACE:            return "LBRACE";
        case TokenType::RBRACE:            return "RBRACE";
        case TokenType::SEMICOLON:         return "SEMICOLON";
        case TokenType::END_OF_FILE:       return "EOF";
        default:                           return "UNKNOWN";
    }
}