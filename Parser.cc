//
// Parser.cc
// Copyright (C) 2021 Richard Bradley
//

// TODO - remove AST_LIST nodes; AST_ITEM (with args as children) can replace it
// TODO - add 'include' directive, evaluated in loadFile()

#include "Parser.hh"
#include "Keywords.hh"
#include "Tokenizer.hh"
#include "Print.hh"
#include <fstream>
#include <sstream>
#include <memory>
#include <cctype>
#include <cassert>

namespace {
  struct ParseError {
    std::string msg;
    int file_id;
    int line;
  };
}


// **** AstNode Class ****
std::string AstNode::desc() const
{
  std::ostringstream os;
  switch (type) {
    case AST_LIST:   os << "<list>"; break;
    case AST_ITEM:   os << "<item " << val << '>'; break;
    case AST_NUMBER: os << "<number " << val << '>'; break;
    case AST_STRING: os << "<string " << val << '>'; break;
    default:
      if (val.empty()) {
        os << "<*null*>";
      } else {
        os << "<*unknown* " << val << '>';
      }
      break;
  }
  return os.str();
}


// **** SceneParser Class ****
int SceneParser::loadFile(const std::string& file)
{
  std::ifstream fs(file);
  if (!fs) { return -1; }

  _files[++_lastID] = file;
  const int baseID = _lastID;

  Tokenizer tk;
  tk.init(fs);

  // parse blocks
  _astList.purge();
  try {
    for (;;) {
      AstNode* n = nextBlock(tk, baseID, 0);
      if (!n) { break; }
      _astList.addToTail(n);
    }
  } catch (ParseError& ex) {
    println_err("[", fileName(ex.file_id), ":", ex.line, "] ", ex.msg);
    return -1;
  } catch (std::exception& ex) {
    println_err("exception: ", ex.what());
    return -1;
  }

  // everything okay
  return 0;
}

AstNode* SceneParser::nextBlock(Tokenizer& tk, int fileID, int depth)
{
  SList<AstNode> nodeList;
  for (;;) {
    int line = 0;
    std::string token;
    TokenType type = tk.getToken(token, line);
    if (type == TOKEN_EOF) {
      if (depth > 0) {
        throw ParseError{"Unexpected end of file", fileID, line};
      }
      break; // done with parse
    } else if (type == TOKEN_RPARAN) {
      break; // end of block
    }

    auto n = std::make_unique<AstNode>(token);
    if (type == TOKEN_LPARAN) {
      n->type = AST_LIST;
      n->child = nextBlock(tk, fileID, depth + 1);
    } else if (type == TOKEN_SYMBOL) {
      ItemFn fn = FindItemFn(token);
      if (fn) {
        n->type = AST_ITEM;
        n->ptr = (void*)fn;
      }
    } else if (type == TOKEN_NUMBER) {
      n->type = AST_NUMBER;
    } else if (type == TOKEN_STRING) {
      n->type = AST_STRING;
    }

    n->file_id = fileID;
    n->line = line;
    nodeList.addToTail(n.release());
  }

  return nodeList.extractNodes();
}

int SceneParser::setupScene(Scene& s)
{
  return processList(s, nullptr, _astList.head());
}

int SceneParser::processList(
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

int SceneParser::processNode(
  Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  if (n->type == AST_LIST) {
    n = n->child;
    assert(n != nullptr);
    if (n->type == AST_ITEM) {
      int er = ItemFn(n->ptr)(*this, s, parent, n->next(), flag);
      if (er) { reportError(n, "Error with ", n->desc()); }
      return er;
    }
  }

  reportError(n, "Unexpected value '", n->val, "'");
  return -1;
}

int SceneParser::getBool(AstNode*& n, bool& val) const
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
      reportError(d, "Expected boolean value, not '", d->val, "'");
      return -1;
  }

  return 0;
}

int SceneParser::getString(AstNode*& n, std::string& val) const
{
  if (!n) { return -1; }
  val = n->val;
  n = n->next();
  return 0;
}

int SceneParser::getFlt(AstNode*& n, Flt& val) const
{
  if (!n) { return -1; }

  AstNode* d = n;
  n = n->next();

  if (d->type != AST_NUMBER) {
    reportError(d, "Expected floating point value, not '", d->val, "'");
    return -1;
  }

  val = std::stof(d->val);
  return 0;
}

int SceneParser::getInt(AstNode*& n, int& val) const
{
  if (!n) { return -1; }

  AstNode* d = n;
  n = n->next();

  if (d->type != AST_NUMBER) {
    reportError(d, "Expected integer value, not '", d->val, "'");
    return -1;
  }

  val = std::stoi(d->val);
  return 0;
}

int SceneParser::getVec3(AstNode*& n, Vec3& v) const
{
  if (int er = getFlt(n, v.x); er != 0) { return er; }
  if (int er = getFlt(n, v.y); er != 0) { return er; }
  if (int er = getFlt(n, v.z); er != 0) { return er; }
  return 0;
}
