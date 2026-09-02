#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

int main(int argc, char* argv[]) {
    std::string inputFilePath = (argc > 1) ? argv[1] : "examples/sample.bn";
    std::string outputFilePath = (argc > 2) ? argv[2] : "output.py";

    //1. Read the source file
    std::ifstream inputFile(inputFilePath, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "[Error] Could not open input file: " << inputFilePath << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();
    inputFile.close();

    std::cout << "[INFO] Compiling '" << inputFilePath << "'...\n";

    try {
        //2.Lexical Analysis
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenizeAll();
        std::cout << "[INFO] Lexer completed. Total tokens: " << tokens.size() << "\n";

        //3.Parsing & AST Construction
        Parser parser(std::move(tokens));
        std::unique_ptr<ProgramNode> astRoot = parser.parseProgram();
        std::cout << "[INFO] Parser completed successfully.\n";

        //4.Code Generation (Target: Python)
        CodeGenerator codeGen;
        std::string pythonCode = codeGen.generate(astRoot.get());

        //5.Write to output file
        std::ofstream outputFile(outputFilePath, std::ios::out);
        if (!outputFile.is_open()) {
            std::cerr << "[Error] Could not create output file: " << outputFilePath << "\n";
            return 1;
        }

        outputFile << pythonCode;
        outputFile.close();

        std::cout << "[SUCCESS] Python output generated at: " << outputFilePath << "\n";

    } catch (const std::exception& e) {
        std::cerr << "[Compilation Failed] " << e.what() << "\n";
        return 1;
    }

    return 0;
}