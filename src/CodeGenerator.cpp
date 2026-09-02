#include "CodeGenerator.h"
#include <stdexcept>

void CodeGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        out << "    "; // 4 spaces per Python indentation level
    }
}

std::string CodeGenerator::getIndent() const {
    std::string s;
    for (int i = 0; i < indentLevel; ++i) {
        s += "    ";
    }
    return s;
}

std::string CodeGenerator::generate(const ProgramNode* program) {
    out.str("");
    out.clear();
    indentLevel = 0;

    out << "# Auto-generated Python code by Bangla Compiler (CSE-4114)\n\n";

    for (const auto& stmt : program->statements) {
        generateStatement(stmt.get());
    }

    return out.str();
}

void CodeGenerator::generateStatement(const StatementNode* stmt) {
    if (!stmt) return;

    if (auto decl = dynamic_cast<const VarDeclNode*>(stmt)) {
        generateVarDecl(decl);
    } else if (auto assign = dynamic_cast<const AssignStmtNode*>(stmt)) {
        generateAssign(assign);
    } else if (auto printStmt = dynamic_cast<const PrintStmtNode*>(stmt)) {
        generatePrint(printStmt);
    } else if (auto ifStmt = dynamic_cast<const IfStmtNode*>(stmt)) {
        generateIf(ifStmt);
    } else if (auto whileStmt = dynamic_cast<const WhileStmtNode*>(stmt)) {
        generateWhile(whileStmt);
    } else if (auto block = dynamic_cast<const BlockStmtNode*>(stmt)) {
        generateBlock(block);
    }
}

void CodeGenerator::generateVarDecl(const VarDeclNode* decl) {
    indent();
    out << decl->identifier << " = ";
    if (decl->initializer) {
        out << generateExpression(decl->initializer.get());
    } else {
        // Default initialization if no initial value provided
        if (decl->typeName == "সংখ্যা") {
            out << "0";
        } else if (decl->typeName == "যৌক্তিক") {
            out << "False";
        } else {
            out << "None";
        }
    }
    out << "\n";
}

void CodeGenerator::generateAssign(const AssignStmtNode* assign) {
    indent();
    out << assign->identifier << " = " << generateExpression(assign->value.get()) << "\n";
}

void CodeGenerator::generatePrint(const PrintStmtNode* printStmt) {
    indent();
    out << "print(" << generateExpression(printStmt->expression.get()) << ")\n";
}

void CodeGenerator::generateIf(const IfStmtNode* ifStmt) {
    indent();
    out << "if " << generateExpression(ifStmt->condition.get()) << ":\n";
    
    indentLevel++;
    generateBlock(ifStmt->thenBranch.get());
    indentLevel--;

    if (ifStmt->elseBranch) {
        indent();
        out << "else:\n";
        indentLevel++;
        generateBlock(ifStmt->elseBranch.get());
        indentLevel--;
    }
}

void CodeGenerator::generateWhile(const WhileStmtNode* whileStmt) {
    indent();
    out << "while " << generateExpression(whileStmt->condition.get()) << ":\n";

    indentLevel++;
    generateBlock(whileStmt->body.get());
    indentLevel--;
}

void CodeGenerator::generateBlock(const BlockStmtNode* block) {
    if (!block || block->statements.empty()) {
        indent();
        out << "pass\n";
        return;
    }

    for (const auto& stmt : block->statements) {
        generateStatement(stmt.get());
    }
}

std::string CodeGenerator::generateExpression(const ExpressionNode* expr) {
    if (!expr) return "";

    if (auto bin = dynamic_cast<const BinaryExprNode*>(expr)) {
        return generateBinary(bin);
    }
    if (auto un = dynamic_cast<const UnaryExprNode*>(expr)) {
        return generateUnary(un);
    }
    if (auto num = dynamic_cast<const NumberLiteralNode*>(expr)) {
        return generateNumber(num);
    }
    if (auto b = dynamic_cast<const BooleanLiteralNode*>(expr)) {
        return generateBoolean(b);
    }
    if (auto var = dynamic_cast<const VariableExprNode*>(expr)) {
        return generateVariable(var);
    }

    return "";
}

std::string CodeGenerator::generateBinary(const BinaryExprNode* bin) {
    std::string leftStr = generateExpression(bin->left.get());
    std::string rightStr = generateExpression(bin->right.get());
    
    // Group sub-expressions with parentheses for explicit precedence preservation
    return "(" + leftStr + " " + bin->op + " " + rightStr + ")";
}

std::string CodeGenerator::generateUnary(const UnaryExprNode* un) {
    return "(" + un->op + generateExpression(un->operand.get()) + ")";
}

std::string CodeGenerator::generateNumber(const NumberLiteralNode* num) {
    return std::to_string(num->value);
}

std::string CodeGenerator::generateBoolean(const BooleanLiteralNode* b) {
    return b->value ? "True" : "False";
}

std::string CodeGenerator::generateVariable(const VariableExprNode* var) {
    return var->name;
}