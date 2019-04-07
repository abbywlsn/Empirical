/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2019
 *
 *  @file  concept_gen.cc
 *  @brief A system to generate dynamic concept code for C++17.
 *  @note Status: PLANNING
 */

#include <fstream>
#include <iostream>
#include <string>

#include "../../source/tools/Lexer.h"

class CodeGen {
private:
  std::string filename;
  emp::Lexer lexer;
  emp::vector<emp::Token> tokens;

public:
  CodeGen(std::string in_filename) : filename(in_filename) {
    lexer.AddToken("ID", "[a-zA-Z0-9.]+");         // Identifiers, tokens, and numbers (including dots)
    lexer.AddToken("Whitespace", "[ \t\n\r]+");    // Any form of whitespace.
    lexer.AddToken("String", "\\\"[^\"]*\\\"");    // Literal strings.
    lexer.AddToken("Other", ".");                  // Symbols

    std::ifstream file(filename);
    tokens = lexer.Tokenize(file);
    file.close();
  }

  void PrintLexerState() { lexer.Print(); }
  void TestLexer() { lexer.DebugString("This is    a \"test\"."); }
};

int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cout << "Format: " << argv[0] << " [input file]" << std::endl;
    exit(0);
  }

  CodeGen codegen(argv[1]);
  codegen.PrintLexerState();
  std::cout << std::endl;
  codegen.TestLexer();
}