#!/bin/bash
# Build the Bangla compiler (Linux / Mac / WSL)
g++ -std=c++17 -Wall -Iinclude \
    src/main.cpp src/lexer.cpp src/parser.cpp \
    src/semantic.cpp src/codegen.cpp \
    -o bangla_compiler \
&& echo "Build OK -> ./bangla_compiler"
