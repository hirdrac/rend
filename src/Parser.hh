//
// Parser.hh
// Copyright (C) 2024 Richard Bradley
//
// Scene description file parser
//

#pragma once
#include "SceneItem.hh"
#include "Types.hh"
#include "SList.hh"
#include "Print.hh"
#include <string>
#include <string_view>
#include <map>
#include <set>


// **** Types ****
class Scene;
class Tokenizer;

enum AstType {
  AST_UNKNOWN, AST_LIST, AST_ITEM, AST_NUMBER, AST_STRING
};


class AstNode
{
 public:
  AstNode*    next = nullptr;
  AstNode*    child = nullptr;
  void*       ptr = nullptr;
  std::string val;
  int         file_id;
  int         line;
  int         column;
  AstType     type = AST_UNKNOWN;

  AstNode(std::string_view value) : val{value} { }
  ~AstNode() { KillNodes(child); }

  // Member Functions
  [[nodiscard]] std::string desc() const;
};


class SceneParser
{
 public:
  // Member Functions
  int loadFile(const std::string& file);
  int setupScene(Scene& s);

  // parsing helper functions
  int processList(Scene& s, SceneItem* parent, AstNode* node,
                  SceneItemFlag flag = FLAG_NONE);

  int getBool(AstNode*& n, bool& val) const;
  int getString(AstNode*& n, std::string& val) const;
  int getFlt(AstNode*& n, Flt& val) const;
  int getInt(AstNode*& n, int& val) const;
  int getVec3(AstNode*& n, Vec3& v) const;

  [[nodiscard]] const std::string& fileName(int file_id) const {
    static const std::string unknown{"<unknown>"};
    const auto itr = _files.find(file_id);
    return (itr == _files.end()) ? unknown : itr->second;
  }

  template<class... Args>
  void reportError(const AstNode* n, const Args&... args) const {
    println_err("[", fileName(n->file_id), ":", n->line, ":", n->column, "] ",
                args...);
  }

 private:
  SList<AstNode> _astList;
  std::map<int,std::string,std::less<>> _files;
  std::set<std::string,std::less<>> _activeFiles;
  int _lastID = 0;

  int includeFile(
    const std::string& file, SList<AstNode>& nodeList, const AstNode* srcNode);
  [[nodiscard]] AstNode* nextBlock(Tokenizer& tk, int fileID, int depth);
  [[nodiscard]] bool evalSpecialOp(
    const AstNode* n, SList<AstNode>& nodeList, int depth);
  int processNode(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag);
};
