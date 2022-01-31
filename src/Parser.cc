//
// Parser.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Parser.hh"
#include "Keywords.hh"
#include "Tokenizer.hh"
#include "Print.hh"
#include <fstream>
#include <sstream>
#include <memory>
#include <cctype>

namespace {
  struct ParseError {
    std::string msg;
    int file_id;
    int line;
    int column;
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
  try {
    _astList.purge();
    return includeFile(file, _astList, nullptr);
  } catch (ParseError& ex) {
    if (ex.file_id) {
      print_err("[", fileName(ex.file_id), ":", ex.line, ":", ex.column, "] ");
    }
    println_err(ex.msg);
    return -1;
  } catch (std::exception& ex) {
    println_err("exception: ", ex.what());
    return -1;
  }
}

int SceneParser::includeFile(
  const std::string& file, SList<AstNode>& nodeList, const AstNode* srcNode)
{
  int src_fileID = 0, src_line = 0, src_column = 0;
  if (srcNode) {
    src_fileID = srcNode->file_id;
    src_line = srcNode->line;
    src_column = srcNode->column;
  }

  if (_activeFiles.count(file) > 0) {
    throw ParseError{
      "Circular include error", src_fileID, src_line, src_column};
  }

  std::ifstream fs(file);
  if (!fs) {
    throw ParseError{
      "Cannot open file '"+file+"'", src_fileID, src_line, src_column};
  }

  _activeFiles.insert(file);
  _files[++_lastID] = file;
  const int baseID = _lastID;

  Tokenizer tk(fs);
  while (AstNode* n = nextBlock(tk, baseID, 0)) {
    nodeList.addToTail(n);
  }

  _activeFiles.erase(file);
  return 0;
}

AstNode* SceneParser::nextBlock(Tokenizer& tk, int fileID, int depth)
{
  SList<AstNode> nodeList;
  for (;;) {
    int line = 0, column = 0;
    std::string token;
    const TokenType type = tk.getToken(token, line, column);
    if (type == TOKEN_EOF) {
      if (depth > 0) {
        throw ParseError{"Unexpected end of file", fileID, line, column};
      }
      break; // done with parse
    } else if (type == TOKEN_RPARAN) {
      break; // end of block
    }

    auto n = std::make_unique<AstNode>(token);
    if (type == TOKEN_LPARAN) {
      n->child = nextBlock(tk, fileID, depth + 1);
      if (evalSpecialOp(n->child, nodeList, depth)) { continue; }
      n->type = AST_LIST;

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
    n->column = column;
    nodeList.addToTail(n.release());
  }

  return nodeList.extractNodes();
}

bool SceneParser::evalSpecialOp(
  const AstNode* n, SList<AstNode>& nodeList, int depth)
{
  if (!n || n->type != AST_UNKNOWN) { return false; }

  const AstNode* n2 = n->next;
  if (n->val == "include") {
    if (!n2 || n2->type != AST_STRING || n2->next) {
      throw ParseError{"'include' syntax error", n->file_id, n->line, n->column};
    }

    const std::string file = fileName(n->file_id);
    std::string baseDir;
    if (auto x = file.rfind('/'); x != std::string::npos) {
      baseDir = file.substr(0,x+1);
    }

    includeFile(baseDir + n2->val, nodeList, n);
    return true;
  }

  return false;
}

int SceneParser::setupScene(Scene& s)
{
  return processList(s, nullptr, _astList.head());
}

int SceneParser::processList(
  Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  while (n) {
    const int error = processNode(s, parent, n, flag);
    if (error) { return error; }
    n = n->next;
  }
  return 0;
}

int SceneParser::processNode(
  Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag)
{
  if (n->type == AST_LIST) {
    if (n->child == nullptr) {
      reportError(n, "Empty statement");
      return -1;
    }

    n = n->child;
    if (n->type == AST_ITEM) {
      int er = ItemFn(n->ptr)(*this, s, parent, n->next, flag);
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

  switch (n->val[0]) {
    case '0': case 'f': case 'F': case 'n': case 'N':
      val = false; break;

    case '1': case 't': case 'T': case 'y': case 'Y':
      val = true; break;

    default:
      reportError(n, "Expected boolean value, not '", n->val, "'");
      return -1;
  }

  n = n->next;
  return 0;
}

int SceneParser::getString(AstNode*& n, std::string& val) const
{
  if (!n) { return -1; }

  val = n->val;
  n = n->next;
  return 0;
}

int SceneParser::getFlt(AstNode*& n, Flt& val) const
{
  if (!n) { return -1; }

  if (n->type != AST_NUMBER) {
    reportError(n, "Expected floating point value, not '", n->val, "'");
    return -1;
  }

  val = std::stod(n->val);
  n = n->next;
  return 0;
}

int SceneParser::getInt(AstNode*& n, int& val) const
{
  if (!n) { return -1; }

  if (n->type != AST_NUMBER) {
    reportError(n, "Expected integer value, not '", n->val, "'");
    return -1;
  }

  val = std::stoi(n->val);
  n = n->next;
  return 0;
}

int SceneParser::getVec3(AstNode*& n, Vec3& v) const
{
  if (int er = getFlt(n, v.x); er != 0) { return er; }
  if (int er = getFlt(n, v.y); er != 0) { return er; }
  if (int er = getFlt(n, v.z); er != 0) { return er; }
  return 0;
}
