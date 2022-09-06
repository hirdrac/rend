//
// Tokenizer.cc
// Copyright (C) 2022 Richard Bradley
//

// TODO: add utf8/unicode support

#include "Tokenizer.hh"
#include <cctype>


[[nodiscard]] static bool isNumber(const std::string& val)
{
  const char* p = val.c_str();
  if (*p == '-') { ++p; }
  if (*p == '.') { ++p; }
  return std::isdigit(*p);
}


// **** Tokenizer class ****
void Tokenizer::init(std::istream& input)
{
  _input = &input;
  _line = 1;
  _column = 1;
  readNext();
}

TokenType Tokenizer::getToken(Token& tk)
{
  if (!_input) { return TOKEN_EOF; }

  for (;;) {
    // start of token read
    while (std::isspace(_nextChar)) { getChar(); }
    tk.line = _line;
    tk.column = _column;

    int c = getChar();
    if (c == '\0') {
      return TOKEN_EOF;
    } else if (c == '\"' || c == '\'') {
      const int quote = c;
      c = getChar();
      if (c != quote) {
        tk.value = char(c);
        // read rest of string
        while (_nextChar != quote && _nextChar != '\0') {
          tk.value += char(getChar());
        }
        getChar(); // skip ending quote
      }
      return TOKEN_STRING;
    } else if (c == ';' || c == '#') {
      // lisp/shell style comment - skip to end of line
      do { c = getChar(); } while ((c != '\0') && (c != '\n'));
      continue; // restart token read
    } else if (c == '/') {
      // check for 'C++' style comments
      if (_nextChar == '/') {
        // Skip rest of line
        do { c = getChar(); } while ((c != '\0') && (c != '\n'));
      } else if (_nextChar == '*') {
        // Skip rest of comment
        getChar(); // skip '*'
        do {
          c = getChar();
        } while ((c != '\0') && !((c == '*') && (_nextChar == '/')));
        getChar(); // skip '/'
      }
      continue; // restart token read
    }

    tk.value = char(c);
    if (c == '(') { return TOKEN_LPARAN; }
    else if (c == ')') { return TOKEN_RPARAN; }

    // read rest of token
    if (std::isalnum(c) || (c == '-') || (c == '.') || (c == '_')) {
      while (std::isalnum(_nextChar) || (_nextChar == '-')
             || (_nextChar == '.') || (_nextChar == '_')) {
        tk.value += char(getChar());
      }
    }

    return isNumber(tk.value) ? TOKEN_NUMBER : TOKEN_SYMBOL;
  }
}

int Tokenizer::getChar()
{
  // One character pipeline used
  // (to make it easy to look ahead one character)
  if (_input->eof()) {
    _nextChar = '\0';
    return _nextChar;
  }

  const int c = _nextChar;
  if (c == '\n') { ++_line; _column = 1; }
  else if (c != '\0') { ++_column; }

  readNext();
  return c;
}

void Tokenizer::readNext()
{
  do { _nextChar = _input->get(); }
  while ((_nextChar == '\r' || _nextChar == '\0') && !_input->eof());
}
