// semantic.h - Phase 3: checks the meaning (variables and types)
#pragma once
#include <string>
#include "ast.h"
#include <map>

class SemanticAnalyzer {
public:
    bool analyze(NodePtr program);
    int errors() const;

private:
    std::map<std::string, Type> symbols;   // symbol table: variable name -> type
    int errorCount = 0;

    void error(NodePtr node, const std::string& message);
    void checkBlock(NodePtr block);
    void checkStatement(NodePtr n);
    void checkCondition(NodePtr cond);
    void checkAssignable(NodePtr node, Type target, Type value);
    Type checkExpr(NodePtr n);
    bool isZero(NodePtr n);
};
