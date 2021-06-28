//
// Parse.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of a parse module
//

// TODO - remove AST_LIST nodes; AST_ITEM (with args as children) can replace it
// TODO - add 'include' directive, evaluated in parseFile()
// TODO - add scene filename to error messages

#include "Parse.hh"
#include "Keywords.hh"
#include "Print.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cassert>


namespace {
  class BlockReader
  {
   public:
    BlockReader(const std::string& file) : _fs(file) { }
    explicit operator bool() const { return _fs.good(); }

    [[nodiscard]] AstNode* nextBlock();

   private:
    std::ifstream _fs;
    int _line = 1;
    int _nextChar = ' ';

    std::string nextToken();
    int getChar();
  };

  AstNode* BlockReader::nextBlock()
  {
    SList<AstNode> nodeList;
    for (;;) {
      std::string token = nextToken();
      if (token.empty() || token[0] == ')') { break; }

      AstNode* e;
      if (token[0] == '(') {
        e = new AstNode(_line);
        e->child = nextBlock();
      } else {
        e = new AstNode(_line, token);
      }

      e->setType();
      nodeList.addToTail(e);
    }

    return nodeList.extractNodes();
  }

  std::string BlockReader::nextToken()
  {
    int quote = 0;
    std::string token;

    for (;;) {
      int c;
      do { c = getChar(); } while (!quote && std::isspace(c));
        // skip initial white-space if not quoted

      // Check 1st character
      if (c == '\0') {
        return token;  // end of file
      } else if (quote) {
        if (c == quote) {
          return token.empty() ? std::string("\"\"") : token;
        } else if (c == '\"' || c == '\'') {
          quote = c;
          continue;
        }
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

      token += char(c); // doesn't support unicode
      if (quote) { continue; }

      // skip to end of token
      if (std::isalnum(c) || (c == '+') || (c == '-') || (c == '.') || (c == '_')) {
	while (std::isalnum(_nextChar) || (_nextChar == '.') || (_nextChar == '_')) {
	  token += char(getChar());
	}
      }

      return token;
    }
  }

  int BlockReader::getChar()
  {
    // One character pipeline used
    // (to make it easy to look ahead one character)
    int c = _nextChar;
    if (!_fs.eof()) {
      if (c == '\n') { ++_line; }
      _nextChar = _fs.get();
    } else {
      c = _nextChar = '\0';
    }

    return c;
  }
}


/**** AstNode Class ****/
// Member functions
std::string AstNode::desc(int indent) const
{
  if (ast_type == AST_LIST) {
    return "<List>";
  }

  std::ostringstream os;
  switch (ast_type) {
    case AST_NUMBER: os << "<Number " << val << '>'; break;
    case AST_ITEM:   os << "<Keyword " << val << '>'; break;
    default:         os << "<Symbol " << val << '>'; break;
  }

  if (child) {
    os << '\n';
    PrintList(os, child, indent + 2);
  }

  return os.str();
}

void AstNode::setType()
{
  if (child) {
    ast_type = AST_LIST;
  } else if (std::isdigit(val[0]) || val[0] == '-' || val[0] == '.') {
    ast_type = AST_NUMBER;
  } else if (ItemFn fn = FindItemFn(val); fn) {
    ast_type = AST_ITEM;
    ptr      = (void*) fn;
  }
}


/**** SceneDesc Class ****/
// Member Functions
int SceneDesc::parseFile(const std::string& file)
{
  BlockReader br(file);
  if (!br) { return -1; }

  // parse blocks
  _astList.purge();
  for (;;) {
    AstNode* n = br.nextBlock();
    if (!n) { break; }
    _astList.addToTail(n);
  }

  // everything okay
  return 0;
}

int SceneDesc::setupScene(Scene& s) const
{
  return ProcessList(s, nullptr, _astList.head());
}


// **** Function Implementations ****
namespace {
  template<typename... Args>
  inline void reportError(int line, const Args&... args)
  {
    println_err("Error in line ", line, ": ", args...);
  }

  int processNode(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
  {
    if (n->ast_type == AST_LIST) {
      n = n->child;
      assert(n != nullptr);
      if (n->ast_type == AST_ITEM) {
        return ItemFn(n->ptr)(s, parent, n->next(), flag);
      }
    }

    reportError(n->line, "Unexpected value '", n->val, "'");
    return -1;
  }
}

int ProcessList(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  int error = 0;
  while (n) {
    error = processNode(s, parent, n, flag);
    if (error) { break; }
    n = n->next();
  }

  return error;
}

int GetBool(AstNode*& n, bool& val, bool def)
{
  if (!n) {
    val = def;
    return 0;
  }

  AstNode* d = n;
  n = n->next();

  if (d->val.empty()) {
    val = def;
    return 0;
  }

  switch (d->val[0]) {
    case '0': case 'f': case 'F': case 'n': case 'N':
      val = false; break;

    case '1': case 't': case 'T': case 'y': case 'Y':
      val = true; break;

    default:
      reportError(d->line, "Expected boolean value");
      val = def;
      return -1;
  }

  return 0;
}

int GetString(AstNode*& n, std::string& val, const std::string& def)
{
  if (!n) {
    val = def;
    return 0;
  }

  val = n->val.empty() ? def : n->val;
  n = n->next();
  return 0;
}

int GetFlt(AstNode*& n, Flt& val, Flt def)
{
  if (!n)  {
    val = def;
    return 0;
  }

  AstNode* d = n;
  n = n->next();

  if (d->ast_type != AST_NUMBER) {
    reportError(d->line, "Expected floating point value");
    val = def;
    return -1;
  }

  val = std::stof(d->val);
  return 0;
}

int GetInt(AstNode*& n, int& val, int def)
{
  if (!n) {
    val = def;
    return 0;
  }

  AstNode* d = n;
  n = n->next();

  if (d->ast_type != AST_NUMBER) {
    reportError(d->line, "Expected integer value");
    val = def;
    return -1;
  }

  val = std::stoi(d->val);
  return 0;
}

int GetVec2(AstNode*& n, Vec2& v, const Vec2& def)
{
  if (int er = GetFlt(n, v.x, def.x); er != 0) { return er; }
  if (int er = GetFlt(n, v.y, def.y); er != 0) { return er; }
  return 0;
}

int GetVec3(AstNode*& n, Vec3& v, const Vec3& def)
{
  if (int er = GetFlt(n, v.x, def.x); er != 0) { return er; }
  if (int er = GetFlt(n, v.y, def.y); er != 0) { return er; }
  if (int er = GetFlt(n, v.z, def.z); er != 0) { return er; }
  return 0;
}
