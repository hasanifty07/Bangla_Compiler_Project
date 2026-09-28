// semantic.cpp - Phase 3: meaning of the program
//   - symbol table (a map: variable name -> type): declared before use? only once?
//   - type checking on assignment
//   - type of every expression
//   - division by zero
#include "semantic.h"
#include <iostream>
using namespace std;

bool SemanticAnalyzer::analyze(NodePtr program) {
    for (NodePtr statement : program->children) checkStatement(statement);
    return errorCount == 0;
}

int SemanticAnalyzer::errors() const { return errorCount; }

void SemanticAnalyzer::error(NodePtr node, const string& message) {
    cout << "Line " << node->line << ": semantic error: " << message << endl;
    errorCount++;
}

void SemanticAnalyzer::checkBlock(NodePtr block) {
    for (NodePtr statement : block->children) checkStatement(statement);
}

void SemanticAnalyzer::checkStatement(NodePtr n) {
    switch (n->kind) {
        case N_DECLARE:
            if (symbols.count(n->text))
                error(n, "variable '" + n->text + "' is already declared");
            else
                symbols[n->text] = n->type;
            if (!n->children.empty())
                checkAssignable(n, n->type, checkExpr(n->children[0]));
            break;

        case N_ASSIGN: {
            Type valueType = checkExpr(n->children[0]);
            if (!symbols.count(n->text)) {
                error(n, "variable '" + n->text + "' is not declared");
            } else {
                n->type = symbols[n->text];   // remember the variable's type
                checkAssignable(n, n->type, valueType);
            }
            break;
        }

        case N_IF:
            checkCondition(n->children[0]);
            checkBlock(n->children[1]);
            if (n->children.size() > 2) checkBlock(n->children[2]);
            break;

        case N_WHILE:
            checkCondition(n->children[0]);
            checkBlock(n->children[1]);
            break;

        case N_PRINT:
            for (NodePtr item : n->children)
                if (item->kind != N_STRING) checkExpr(item);
            break;

        default: break;
    }
}

void SemanticAnalyzer::checkCondition(NodePtr cond) {
    checkExpr(cond->children[0]);
    checkExpr(cond->children[1]);
}

// purno can go into doshomik, but doshomik cannot go into purno
void SemanticAnalyzer::checkAssignable(NodePtr node, Type target, Type value) {
    if (target == TYPE_INT && value == TYPE_FLOAT)
        error(node, "type mismatch: cannot store a doshomik value in purno variable '"
                    + node->text + "'");
}

// Finds the type of an expression and saves it in the node.
Type SemanticAnalyzer::checkExpr(NodePtr n) {
    switch (n->kind) {
        case N_INT:   n->type = TYPE_INT;   break;
        case N_FLOAT: n->type = TYPE_FLOAT; break;

        case N_VAR:
            if (!symbols.count(n->text)) {
                error(n, "variable '" + n->text + "' is not declared");
                n->type = TYPE_INT;          // guess, so we do not report more errors
            } else {
                n->type = symbols[n->text];
            }
            break;

        case N_NEGATE:
            n->type = checkExpr(n->children[0]);
            break;

        case N_BINARY: {
            Type left  = checkExpr(n->children[0]);
            Type right = checkExpr(n->children[1]);
            if (n->text == "/" && isZero(n->children[1]))
                error(n, "division by zero");
            // purno + doshomik -> doshomik
            n->type = (left == TYPE_FLOAT || right == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
            break;
        }
        default: break;
    }
    return n->type;
}

// is this a number literal like 0 or 0.0 ?
bool SemanticAnalyzer::isZero(NodePtr n) {
    if (n->kind != N_INT && n->kind != N_FLOAT) return false;
    for (char c : n->text)
        if (c != '0' && c != '.') return false;
    return true;
}
