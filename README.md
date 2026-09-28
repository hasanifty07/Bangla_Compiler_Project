# Bangla Compiler (CSE-4114)

A compiler for a toy Bangla language (written in Banglish so it is easy to type).
It translates `.bangla` source code into a **Python** file. The compiler is written in **C++**.

## Project structure

    bangla_compiler/
    ├── include/               header files (.h): what each class looks like
    │   ├── ast.h              AST node (the tree)
    │   ├── lexer.h            Token, Lexer
    │   ├── parser.h           Parser
    │   ├── semantic.h         SemanticAnalyzer
    │   └── codegen.h          CodeGenerator
    ├── src/                   source files (.cpp): how each class works
    │   ├── lexer.cpp
    │   ├── parser.cpp
    │   ├── semantic.cpp
    │   ├── codegen.cpp
    │   └── main.cpp           runs the 4 phases
    ├── examples/              syntax_errors.bangla, semantic_errors.bangla
    ├── program.bangla         sample program
    ├── build.bat              build on Windows
    ├── build.sh               build on Linux / Mac / WSL
    └── README.md

## Build and run

Windows:

    build.bat
    bangla_compiler program.bangla output.py
    python output.py

Linux / Mac / WSL:

    ./build.sh
    ./bangla_compiler program.bangla output.py
    python3 output.py

(In PowerShell use `.\bangla_compiler` instead of `bangla_compiler`.)

## Language reference

| Bangla keyword | Meaning            | Example                      |
|----------------|--------------------|------------------------------|
| `purno`        | integer type       | `purno x = 5;`               |
| `doshomik`     | decimal type       | `doshomik pi = 3.14;`        |
| `jodi`         | if                 | `jodi (x > 3) { ... }`       |
| `nahole`       | else               | `nahole { ... }`             |
| `jotokkhon`    | while              | `jotokkhon (x < 10) { ... }` |
| `dekhao`       | print              | `dekhao("x = ", x);`         |

- Operators: `+ - * /` (`*` and `/` bind tighter than `+` and `-`), comparisons `< > <= >= == !=`
- Comments start with `#`
- A `purno` value can go into a `doshomik` variable, but **not** the other way round (type error)
- `purno / purno` gives a `purno` result
- Every variable is declared once, before use (one global scope)

## Compiler architecture

    Source (.bangla)
        |
        v
    [1] Lexer             text   -> tokens
        |
        v
    [2] Parser            tokens -> AST          (syntax errors, recovers at ';')
        |
        v
    [3] SemanticAnalyzer  AST    -> typed AST    (symbol table map; type checking)
        |
        v
    [4] CodeGenerator     AST    -> Python (.py)

If phase 2 finds errors, the compiler stops before phase 3.
If phase 3 finds errors, no Python file is written.

## Requirements checklist

| Requirement                          | Where                                                    |
|--------------------------------------|----------------------------------------------------------|
| Two data types + type checking       | `semantic.cpp`: `checkAssignable()`, `checkExpr()`       |
| Arithmetic with precedence           | `parser.cpp`: `expression()` -> `term()` -> `factor()`   |
| Assignment                           | `parser.cpp`: `assignment()`, `declaration()`            |
| IF-ELSE                              | `parser.cpp`: `ifStatement()`                            |
| WHILE                                | `parser.cpp`: `whileStatement()`                         |
| Syntax error recovery (to `;`)       | `parser.cpp`: `parseStatementSafely()`                   |
| No crashes                           | safe token reading, nesting/size limits, file checks     |
| Generates executable target file     | `codegen.cpp`, written to disk by `main.cpp`             |

## Classes (for the UML diagram in your report)

    Token             : type, text, line
    Lexer             : src, i, line
                        tokenize(), nextToken(), readNumber(), readWord(), readString()

    Node (AST)        : kind, text, line, type, children
    Parser            : tokens, pos, errorCount, depth, exprNodes
                        parse(), parseStatementSafely(), declaration(), assignment(),
                        ifStatement(), whileStatement(), printStatement(), block(),
                        condition(), expression(), term(), factor()
    SemanticAnalyzer  : symbols (map: name -> type), errorCount
                        analyze(), checkStatement(), checkExpr(), checkAssignable()
    CodeGenerator     : lines, indent
                        generate(), genStatement(), genExpr(), genBlock(), convert()

Relations: `Lexer` produces `Token`s -> `Parser` uses `Token`s and creates `Node`s ->
`SemanticAnalyzer` (which keeps the symbol table) and `CodeGenerator` both read the `Node` tree.

## Grammar (BNF)

    <program>     ::= { <statement> }
    <statement>   ::= <declaration> | <assignment> | <if_stmt> | <while_stmt> | <print_stmt>

    <declaration> ::= <type> IDENT [ "=" <expr> ] ";"
    <type>        ::= "purno" | "doshomik"
    <assignment>  ::= IDENT "=" <expr> ";"
    <if_stmt>     ::= "jodi" <condition> <block> [ "nahole" <block> ]
    <while_stmt>  ::= "jotokkhon" <condition> <block>
    <print_stmt>  ::= "dekhao" "(" <item> { "," <item> } ")" ";"
    <item>        ::= STRING | <expr>

    <block>       ::= "{" { <statement> } "}"
    <condition>   ::= "(" <expr> <relop> <expr> ")"
    <relop>       ::= "<" | ">" | "<=" | ">=" | "==" | "!="

    <expr>        ::= <term> { ( "+" | "-" ) <term> }
    <term>        ::= <factor> { ( "*" | "/" ) <factor> }
    <factor>      ::= INT_LIT | FLOAT_LIT | IDENT | "(" <expr> ")" | "-" <factor>

    IDENT     ::= letter { letter | digit | "_" }
    INT_LIT   ::= digit { digit }
    FLOAT_LIT ::= digit { digit } "." digit { digit }
    STRING    ::= '"' { any character except '"' } '"'
