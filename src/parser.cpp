#include "Parser.h"
#include <stdexcept>
#include <iostream>

// ==========================================
// AST Pretty Printing Implementation
// ==========================================
static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

void ProgramNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ProgramNode\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

void BlockStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BlockStmtNode\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

void VarDeclNode::print(int indent) const {
    printIndent(indent);
    std::cout << "VarDeclNode [Type: " << typeName << ", ID: " << identifier << "]\n";
    if (initializer) {
        initializer->print(indent + 1);
    }
}

void AssignStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "AssignStmtNode [ID: " << identifier << "]\n";
    value->print(indent + 1);
}

void IfStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "IfStmtNode\n";
    printIndent(indent + 1);
    std::cout << "Condition:\n";
    condition->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Then:\n";
    thenBranch->print(indent + 2);
    if (elseBranch) {
        printIndent(indent + 1);
        std::cout << "Else:\n";
        elseBranch->print(indent + 2);
    }
}

void WhileStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "WhileStmtNode\n";
    printIndent(indent + 1);
    std::cout << "Condition:\n";
    condition->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Body:\n";
    body->print(indent + 2);
}

// 1. AST node print method for PrintStmtNode
void PrintStmtNode::print(int indent) const {
    printIndent(indent);
    std::cout << "PrintStmtNode\n";
    expression->print(indent + 1);
}

void BinaryExprNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BinaryExprNode (" << op << ")\n";
    left->print(indent + 1);
    right->print(indent + 1);
}

void UnaryExprNode::print(int indent) const {
    printIndent(indent);
    std::cout << "UnaryExprNode (" << op << ")\n";
    operand->print(indent + 1);
}

void NumberLiteralNode::print(int indent) const {
    printIndent(indent);
    std::cout << "NumberLiteralNode (" << value << " / " << rawLexeme << ")\n";
}

void BooleanLiteralNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BooleanLiteralNode (" << (value ? "true" : "false") << " / " << rawLexeme << ")\n";
}

void VariableExprNode::print(int indent) const {
    printIndent(indent);
    std::cout << "VariableExprNode (" << name << ")\n";
}

// ==========================================
// Parser Implementation
// ==========================================

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    error(peek(), errorMessage);
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) + ": " + errorMessage);
}

void Parser::error(const Token& token, const std::string& message) const {
    std::cerr << "[Syntax Error] Line " << token.line << ", Col " << token.column
              << " near '" << token.lexeme << "': " << message << "\n";
}

long long Parser::parseBanglaOrAsciiInteger(const std::string& lexeme) {
    // Converts both ASCII (0-9) and UTF-8 Bangla digits (০-৯) into a 64-bit integer
    long long result = 0;
    size_t i = 0;
    while (i < lexeme.size()) {
        unsigned char c = static_cast<unsigned char>(lexeme[i]);
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
            i += 1;
        } else if (c == 0xE0 && i + 2 < lexeme.size()) {
            std::string d = lexeme.substr(i, 3);
            int val = -1;
            if (d == "০") val = 0;
            else if (d == "১") val = 1;
            else if (d == "২") val = 2;
            else if (d == "৩") val = 3;
            else if (d == "৪") val = 4;
            else if (d == "৫") val = 5;
            else if (d == "৬") val = 6;
            else if (d == "৭") val = 7;
            else if (d == "৮") val = 8;
            else if (d == "৯") val = 9;

            if (val != -1) {
                result = result * 10 + val;
                i += 3;
            } else {
                i += 1;
            }
        } else {
            i += 1;
        }
    }
    return result;
}

// <program> ::= <statement_list>
std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        program->statements.push_back(parseStatement());
    }
    return program;
}

// 2. Add dispatch in parseStatement()
// <statement> ::= <declaration_stmt> | <assignment_stmt> | <if_stmt> | <while_stmt> | <print_stmt> | <block_stmt>
std::unique_ptr<StatementNode> Parser::parseStatement() {
    if (match(TokenType::KEYWORD_DHORI)) {
        return parseDeclaration();
    }
    if (match(TokenType::KEYWORD_LEKHO)) {
        return parsePrintStatement();
    }
    if (match(TokenType::KEYWORD_JODI)) {
        return parseIfStatement();
    }
    if (match(TokenType::KEYWORD_JOTOKKHON)) {
        return parseWhileStatement();
    }
    if (match(TokenType::LBRACE)) {
        return parseBlock();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }

    error(peek(), "Expected statement");
    advance();
    return nullptr;
}

// <declaration_stmt> ::= "ধরি" <type> <identifier> ["=" <expression>] ";"
std::unique_ptr<VarDeclNode> Parser::parseDeclaration() {
    // 1. Parse Type: "সংখ্যা" | "যৌক্তিক"
    if (!check(TokenType::KEYWORD_SONGKHA) && !check(TokenType::KEYWORD_JOUKTIK)) {
        error(peek(), "Expected data type ('সংখ্যা' or 'যৌক্তিক') after 'ধরি'");
    }
    std::string typeName = advance().lexeme;

    // 2. Parse Identifier
    const Token& idToken = consume(TokenType::IDENTIFIER, "Expected identifier after type name");
    std::string identifier = idToken.lexeme;

    // 3. Optional initialization
    std::unique_ptr<ExpressionNode> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseCondition(); // Allow expressions and boolean/relational values
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::make_unique<VarDeclNode>(typeName, identifier, std::move(initializer));
}

// <assignment_stmt> ::= <identifier> "=" <expression> ";"
std::unique_ptr<AssignStmtNode> Parser::parseAssignment() {
    const Token& idToken = consume(TokenType::IDENTIFIER, "Expected identifier");
    consume(TokenType::ASSIGN, "Expected '=' in assignment statement");
    auto value = parseCondition();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment statement");
    return std::make_unique<AssignStmtNode>(idToken.lexeme, std::move(value));
}

// 3. Implement parsePrintStatement: <print_stmt> ::= "লেখো" <expression> ";"
std::unique_ptr<PrintStmtNode> Parser::parsePrintStatement() {
    auto expr = parseCondition();
    consume(TokenType::SEMICOLON, "Expected ';' after 'লেখো' statement");
    return std::make_unique<PrintStmtNode>(std::move(expr));
}

// <if_stmt> ::= "যদি" "(" <condition> ")" <block_stmt> ["নাহলে" <block_stmt>]
std::unique_ptr<IfStmtNode> Parser::parseIfStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'যদি'");
    auto condition = parseCondition();
    consume(TokenType::RPAREN, "Expected ')' after if condition");

    consume(TokenType::LBRACE, "Expected '{' for then-block");
    auto thenBranch = parseBlock();

    std::unique_ptr<BlockStmtNode> elseBranch = nullptr;
    if (match(TokenType::KEYWORD_NAHOLE)) {
        consume(TokenType::LBRACE, "Expected '{' for else-block");
        elseBranch = parseBlock();
    }

    return std::make_unique<IfStmtNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

// <while_stmt> ::= "যতক্ষণ" "(" <condition> ")" <block_stmt>
std::unique_ptr<WhileStmtNode> Parser::parseWhileStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'যতক্ষণ'");
    auto condition = parseCondition();
    consume(TokenType::RPAREN, "Expected ')' after while condition");

    consume(TokenType::LBRACE, "Expected '{' for while loop body");
    auto body = parseBlock();

    return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body));
}

// <block_stmt> ::= "{" <statement_list> "}" | "{" "}"
std::unique_ptr<BlockStmtNode> Parser::parseBlock() {
    auto block = std::make_unique<BlockStmtNode>();
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        block->statements.push_back(parseStatement());
    }
    consume(TokenType::RBRACE, "Expected '}' after block");
    return block;
}

// <condition> ::= <expression> [<rel_op> <expression>]
std::unique_ptr<ExpressionNode> Parser::parseCondition() {
    auto expr = parseExpression();

    if (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = parseExpression();
        return std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Level 1 Precedence: Addition & Subtraction (Lowest arithmetic precedence, left-associative)
// <expression> ::= <expression> "+" <term> | <expression> "-" <term> | <term>
std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    auto expr = parseTerm();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Level 2 Precedence: Multiplication & Division (Higher than +/-, left-associative)
// <term> ::= <term> "*" <factor> | <term> "/" <factor> | <factor>
std::unique_ptr<ExpressionNode> Parser::parseTerm() {
    auto term = parseFactor();

    while (match({TokenType::STAR, TokenType::SLASH})) {
        std::string op = previous().lexeme;
        auto right = parseFactor();
        term = std::make_unique<BinaryExprNode>(std::move(term), op, std::move(right));
    }
    return term;
}

// Level 3 Precedence: Unary minus, Parentheses, Literals, Identifiers (Highest precedence)
// <factor> ::= "(" <expression> ")" | "-" <factor> | <literal> | <identifier>
std::unique_ptr<ExpressionNode> Parser::parseFactor() {
    // 1. Unary Minus
    if (match(TokenType::MINUS)) {
        std::string op = previous().lexeme;
        auto operand = parseFactor();
        return std::make_unique<UnaryExprNode>(op, std::move(operand));
    }

    // 2. Parenthesized Expression
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    // 3. Integer Literals (ASCII & Bangla)
    if (match(TokenType::NUMBER)) {
        std::string lexeme = previous().lexeme;
        long long val = parseBanglaOrAsciiInteger(lexeme);
        return std::make_unique<NumberLiteralNode>(lexeme, val);
    }

    // 4. Boolean Literals
    if (match(TokenType::LITERAL_SHOTTO)) {
        return std::make_unique<BooleanLiteralNode>(true, previous().lexeme);
    }
    if (match(TokenType::LITERAL_MITTHA)) {
        return std::make_unique<BooleanLiteralNode>(false, previous().lexeme);
    }

    // 5. Identifiers
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExprNode>(previous().lexeme);
    }

    error(peek(), "Expected factor/expression");
    throw std::runtime_error("Unexpected token in expression: " + peek().lexeme);
}