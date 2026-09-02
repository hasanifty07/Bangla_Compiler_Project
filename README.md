# Bangla to Python Transpiler (CSE-4114)

A fully functional, end-to-end compiler built from scratch in C++ that translates a custom toy Bangla programming language into executable Python code. Developed as the final project for the CSE-4114 Compiler Design sessional.

## 🚀 Features
* **Custom Lexical Analysis & Parsing:** Fully supports UTF-8 Bangla characters for keywords, identifiers, and literals.
* **Abstract Syntax Tree (AST):** Constructs a robust AST to strictly enforce BODMAS/PEMDAS arithmetic precedence and language grammar.
* **Data Types:** Supports dynamic variable assignment for numeric (`সংখ্যা`) and boolean (`যৌক্তিক`) types.
* **Control Flow:** Implements relational logic, `if-else` (`যদি`-`নাহলে`) branching, and `while` (`যতক্ষণ`) loops.
* **Native Output:** Includes a built-in print command (`লেখো`) that maps directly to standard output.

## 🛠️ Tech Stack
* **Compiler Engine:** C++17 (Lexer, Parser, Code Generator)
* **Build System:** Make (MinGW/GCC)
* **Target Environment:** Python 3.x

## 💻 Language Syntax Example
**Input: `examples/sample.bn`**
```text
ধরি সংখ্যা নাম্বার = ৫;
ধরি সংখ্যা ফ্যাক্টোরিয়াল = ১;

যতক্ষণ (নাম্বার > ০) {
    ফ্যাক্টোরিয়াল = ফ্যাক্টোরিয়াল * নাম্বার;
    নাম্বার = নাম্বার - ১;
}

লেখো ফ্যাক্টোরিয়াল;
