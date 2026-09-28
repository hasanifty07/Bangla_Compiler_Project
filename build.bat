@echo off
REM Build the Bangla compiler (Windows)
g++ -std=c++17 -Wall -Iinclude src\main.cpp src\lexer.cpp src\parser.cpp src\semantic.cpp src\codegen.cpp -o bangla_compiler.exe
if %errorlevel%==0 (echo Build OK -^> bangla_compiler.exe) else (echo Build FAILED)
