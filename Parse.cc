//
// Parse.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of a parse module
//

// TODO - remove AST_LIST nodes; AST_ITEM (with args as children) can replace it
// TODO - add 'include' directive, evaluated in parseFile()

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
    BlockReader(const std::string& file, int fileID)
      : _fs(file), _fileID(fileID) { }
    explicit operator bool() const { return _fs.good(); }

    [[nodiscard]] AstNode* nextBlock();

   private:
    std::ifstream _fs;
    int _fileID;
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
        e = new AstNode();
        e->child = nextBlock();
      } else {
        e = new AstNode(token);
      }

      e->file_id = _fileID;
      e->line = _line;
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
    case AST_ITEM:   os << "<Item " << val << '>'; break;
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
  _files[++_lastID] = file;
  BlockReader br(file, _lastID);
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

int SceneDesc::setupScene(Scene& s)
{
  return processList(s, nullptr, _astList.head());
}

int SceneDesc::processList(
  Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  int error = 0;
  while (n) {
    error = processNode(s, parent, n, flag);
    if (error) { break; }
    n = n->next();
  }
  return error;
}

int SceneDesc::processNode(
  Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  if (n->ast_type == AST_LIST) {
    n = n->child;
    assert(n != nullptr);
    if (n->ast_type == AST_ITEM) {
      int er = ItemFn(n->ptr)(*this, s, parent, n->next(), flag);
      if (er) { reportError(n, "Error with ", n->desc(0)); }
      return er;
    }
  }

  reportError(n, "Unexpected value '", n->val, "'");
  return -1;
}

int SceneDesc::getBool(AstNode*& n, bool& val) const
{
  if (!n) { return -1; }

  AstNode* d = n;
  n = n->next();

  switch (d->val[0]) {
    case '0': case 'f': case 'F': case 'n': case 'N':
      val = false; break;

    case '1': case 't': case 'T': case 'y': case 'Y':
      val = true; break;

    default:
      reportError(d, "Expected boolean value");
      return -1;
  }

  return 0;
}

int SceneDesc::getString(AstNode*& n, std::string& val) const
{
  if (!n) { return -1; }
  val = n->val;
  n = n->next();
  return 0;
}

int SceneDesc::getFlt(AstNode*& n, Flt& val) const
{
  if (!n) { return -1; }

  AstNode* d = n;
  n = n->next();

  if (d->ast_type != AST_NUMBER) {
    reportError(d, "Expected floating point value");
    return -1;
  }

  val = std::stof(d->val);
  return 0;
}

int SceneDesc::getInt(AstNode*& n, int& val) const
{
  if (!n) { return -1; }

  AstNode* d = n;
  n = n->next();

  if (d->ast_type != AST_NUMBER) {
    reportError(d, "Expected integer value");
    return -1;
  }

  val = std::stoi(d->val);
  return 0;
}

int SceneDesc::getVec3(AstNode*& n, Vec3& v) const
{
  if (int er = getFlt(n, v.x); er != 0) { return er; }
  if (int er = getFlt(n, v.y); er != 0) { return er; }
  if (int er = getFlt(n, v.z); er != 0) { return er; }
  return 0;
}
