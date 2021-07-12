//
// Tokenizer.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <istream>
#include <string>


enum TokenType {
  TOKEN_EOF,    // end of file

  TOKEN_SYMBOL, // keyword/command
  TOKEN_NUMBER,
  TOKEN_STRING, // quoted string
  TOKEN_LPARAN,
  TOKEN_RPARAN,
};

class Tokenizer
{
 public:
  Tokenizer() = default;
  Tokenizer(std::istream& input) { init(input); }

  void init(std::istream& input);
  [[nodiscard]] TokenType getToken(std::string& value, int& line, int& column);

 private:
  std::istream* _input = nullptr;
  int _line, _column;
  int _nextChar;

  int getChar();
};
