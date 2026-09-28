// codegen.h - Phase 4: turns the checked AST into Python code
#pragma once
#include <string>
#include <vector>
#include "ast.h"

class CodeGenerator {
public:
    std::string generate(NodePtr program);

private:
    std::vector<std::string> lines;
    int indent = 0;

    void emit(const std::string& line);
    void genBlock(NodePtr block);
    void genStatement(NodePtr n);
    std::string genExpr(NodePtr n);
    std::string operand(NodePtr n);
    std::string convert(Type target, NodePtr value);
    std::string escape(const std::string& s);
};
