#include "Lexer.h"
#include <cctype>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"ধরি",     TokenType::KEYWORD_DHORI},
    {"সংখ্যা",   TokenType::KEYWORD_SONGKHA},
    {"যৌক্তিক", TokenType::KEYWORD_JOUKTIK},
    {"যদি",     TokenType::KEYWORD_JODI},
    {"নাহলে",   TokenType::KEYWORD_NAHOLE},
    {"যতক্ষণ",   TokenType::KEYWORD_JOTOKKHON},
    {"লেখো",     TokenType::KEYWORD_LEKHO},
    {"সত্য",     TokenType::LITERAL_SHOTTO},
    {"মিথ্যা",   TokenType::LITERAL_MITTHA}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[cursor];
}

char Lexer::peekNext() const {
    if (cursor + 1 >= source.size()) return '\0';
    return source[cursor + 1];
}

char Lexer::advance() {
    char c = source[cursor++];
    column++;
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[cursor] != expected) return false;
    cursor++;
    column++;
    return true;
}

bool Lexer::isAtEnd() const {
    return cursor >= source.size();
}

size_t Lexer::getUtf8ByteLength(unsigned char leadByte) {
    if ((leadByte & 0x80) == 0x00) return 1;       // 1-byte ASCII (0xxxxxxx)
    if ((leadByte & 0xE0) == 0xC0) return 2;       // 2-byte sequence (110xxxxx)
    if ((leadByte & 0xF0) == 0xE0) return 3;       // 3-byte sequence (1110xxxx, standard Bangla range U+0980..U+09FF)
    if ((leadByte & 0xF8) == 0xF0) return 4;       // 4-byte sequence (11110xxx)
    return 1; // Fallback for malformed bytes
}

bool Lexer::isBanglaLetterOrMark(unsigned char leadByte) {
    // Unicode Bangla Block (U+0980 to U+09FF) encodes in UTF-8 as 3-byte sequences starting with 0xE0
    return leadByte == 0xE0;
}

bool Lexer::isBanglaDigit(const std::string& utf8Char) {
    // Bangla numerals: ০ (E0 A7 A6) through ৯ (E0 A7 AF)
    static const std::vector<std::string> banglaDigits = {
        "০", "১", "২", "৩", "৪", "৫", "৬", "৭", "৮", "৯"
    };
    for (const auto& digit : banglaDigits) {
        if (utf8Char == digit) return true;
    }
    return false;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            line++;
            column = 1;
            cursor++;
        } else if (c == '/' && peekNext() == '/') {
            // Line comment: skip until end of line
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::readNumber() {
    size_t startCol = column;
    size_t startPos = cursor;

    while (!isAtEnd()) {
        unsigned char c = static_cast<unsigned char>(peek());
        if (std::isdigit(c)) {
            advance();
        } else if (c == 0xE0 && cursor + 2 < source.size()) {
            // Check for 3-byte Bangla digit
            std::string possibleDigit = source.substr(cursor, 3);
            if (isBanglaDigit(possibleDigit)) {
                cursor += 3;
                column++;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    std::string lexeme = source.substr(startPos, cursor - startPos);
    return Token(TokenType::NUMBER, lexeme, line, startCol);
}

Token Lexer::readIdentifierOrKeyword() {
    size_t startCol = column;
    size_t startPos = cursor;

    while (!isAtEnd()) {
        unsigned char c = static_cast<unsigned char>(peek());

        if (std::isalnum(c) || c == '_') {
            advance();
        } else if (c >= 0x80) {
            // Consume full UTF-8 codepoint (letters, vowel marks, hasant)
            size_t bytes = getUtf8ByteLength(c);
            if (cursor + bytes <= source.size()) {
                cursor += bytes;
                column++;
            } else {
                advance();
            }
        } else {
            break;
        }
    }

    std::string lexeme = source.substr(startPos, cursor - startPos);
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, line, startCol);
    }
    return Token(TokenType::IDENTIFIER, lexeme, line, startCol);
}

Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }

    size_t startCol = column;
    unsigned char c = static_cast<unsigned char>(peek());

    // 1. Identifiers and Keywords (ASCII letters, underscores, or UTF-8 leading bytes)
    if (std::isalpha(c) || c == '_' || c >= 0x80) {
        // Distinguish Bangla digit from Bangla letter
        if (c == 0xE0 && cursor + 2 < source.size()) {
            std::string sub = source.substr(cursor, 3);
            if (isBanglaDigit(sub)) {
                return readNumber();
            }
        }
        return readIdentifierOrKeyword();
    }

    //2.ASCII Numbers
    if (std::isdigit(c)) {
        return readNumber();
    }

    //3.Operators & Delimiters
    advance();
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", line, startCol);
        case '-': return Token(TokenType::MINUS, "-", line, startCol);
        case '*': return Token(TokenType::STAR, "*", line, startCol);
        case '/': return Token(TokenType::SLASH, "/", line, startCol);
        case ';': return Token(TokenType::SEMICOLON, ";", line, startCol);
        case '(': return Token(TokenType::LPAREN, "(", line, startCol);
        case ')': return Token(TokenType::RPAREN, ")", line, startCol);
        case '{': return Token(TokenType::LBRACE, "{", line, startCol);
        case '}': return Token(TokenType::RBRACE, "}", line, startCol);

        case '=':
            if (match('=')) return Token(TokenType::EQUAL_EQUAL, "==", line, startCol);
            return Token(TokenType::ASSIGN, "=", line, startCol);

        case '!':
            if (match('=')) return Token(TokenType::NOT_EQUAL, "!=", line, startCol);
            return Token(TokenType::UNKNOWN, "!", line, startCol);

        case '<':
            if (match('=')) return Token(TokenType::LESS_EQUAL, "<=", line, startCol);
            return Token(TokenType::LESS, "<", line, startCol);

        case '>':
            if (match('=')) return Token(TokenType::GREATER_EQUAL, ">=", line, startCol);
            return Token(TokenType::GREATER, ">", line, startCol);

        default:
            return Token(TokenType::UNKNOWN, std::string(1, static_cast<char>(c)), line, startCol);
    }
}

std::vector<Token> Lexer::tokenizeAll() {
    std::vector<Token> tokens;
    while (true) {
        Token token = getNextToken();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    return tokens;
}