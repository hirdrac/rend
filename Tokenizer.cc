//
// Tokenizer.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Tokenizer.hh"
#include <cctype>

namespace {
  bool isNumber(const std::string& val)
  {
    const char* p = val.c_str();
    if (*p == '-') { ++p; }
    if (*p == '.') { ++p; }
    return std::isdigit(*p);
  }
}

// **** Tokenizer class ****
void Tokenizer::init(std::istream& input)
{
  _input = &input;
  _line = 1;
  _nextChar = ' ';
}

TokenType Tokenizer::getToken(std::string& value, int& line)
{
  if (!_input) { return TOKEN_EOF; }

  value.clear();
  bool quoted = false;

  int quote = 0;
  for (;;) {
    int c;
    do { c = getChar(); } while (!quote && std::isspace(c));
      // skip initial white-space if not quoted

    // check 1st character
    if (c == '\0' || (quote && c == quote)) {
      break; // end of file/end of quoted range
    } else if (!quote) {
      if (c == '\"' || c == '\'') {
        quote = c;
        quoted = true;
        continue;
      } else if (c == ';' || c == '#') {
        // lisp/shell style comment - skip to end of line
        do { c = getChar(); } while ((c != '\0') && (c != '\n'));
        continue;
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
        continue;
      }
    }

    value += char(c); // doesn't support unicode
    if (quote) { continue; }

    // skip to end of token
    if (std::isalnum(c) || (c == '-') || (c == '.') || (c == '_')) {
      while (std::isalnum(_nextChar) || (_nextChar == '-')
             || (_nextChar == '.') || (_nextChar == '_')) {
        value += char(getChar());
      }
    }
    break;
  }

  line = _line;
  if (quoted) { return TOKEN_STRING; }
  else if (value.empty()) { return TOKEN_EOF; }
  else if (value[0] == '(') { return TOKEN_LPARAN; }
  else if (value[0] == ')') { return TOKEN_RPARAN; }
  else if (isNumber(value)) { return TOKEN_NUMBER; }
  else { return TOKEN_SYMBOL; }
}

bool Tokenizer::eof() const
{
  return !_input || _input->eof();
}

int Tokenizer::getChar()
{
  // One character pipeline used
  // (to make it easy to look ahead one character)
  int c = _nextChar;
  if (!_input->eof()) {
    if (c == '\n') { ++_line; }
    _nextChar = _input->get();
  } else {
    c = _nextChar = '\0';
  }

  return c;
}
