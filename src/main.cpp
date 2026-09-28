// main.cpp - runs the 4 phases one after another
//
//   Usage: bangla_compiler <source.bangla> [output.py]
#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: bangla_compiler <source.bangla> [output.py]" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    if (!inFile) {
        cout << "Error: cannot open file '" << argv[1] << "'" << endl;
        return 1;
    }
    stringstream buffer;
    buffer << inFile.rdbuf();

    // Phase 1: Lexer
    Lexer lexer(buffer.str());
    vector<Token> tokens = lexer.tokenize();

    // Phase 2: Parser (stop if the grammar is wrong)
    Parser parser(tokens);
    NodePtr tree = parser.parse();
    if (parser.errors() > 0) {
        cout << parser.errors() << " syntax error(s) found. No output file created." << endl;
        return 1;
    }

    // Phase 3: Semantic analysis (stop if types / variables are wrong)
    SemanticAnalyzer analyzer;
    if (!analyzer.analyze(tree)) {
        cout << analyzer.errors() << " semantic error(s) found. No output file created." << endl;
        return 1;
    }

    // Phase 4: Code generation
    CodeGenerator generator;
    string python = generator.generate(tree);

    string outName = (argc >= 3) ? argv[2] : "output.py";
    ofstream outFile(outName);
    if (!outFile) {
        cout << "Error: cannot write file '" << outName << "'" << endl;
        return 1;
    }
    outFile << python;
    cout << "Compiled successfully -> " << outName << endl;
    return 0;
}
