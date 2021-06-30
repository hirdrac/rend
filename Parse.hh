//
// Parse.hh
// Copyright (C) 2021 Richard Bradley
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


// **** Types ****
class Scene;

enum AstType {
  AST_UNKNOWN, AST_LIST, AST_ITEM, AST_VALUE,
};


class AstNode : public SListNode<AstNode>
{
 public:
  AstNode*    child = nullptr;
  int         file_id = 0;
  int         line = 0;
  void*       ptr = nullptr;
  std::string val;
  AstType     ast_type = AST_UNKNOWN;
  int         flag = 0;

  // Constructors
  AstNode() = default;
  AstNode(std::string_view value) : val(value) { }

  // Member Functions
  [[nodiscard]] std::string desc(int indent) const;
  void setType();
};


class SceneDesc
{
 public:
  // Member Functions
  int parseFile(const std::string& file);
  int setupScene(Scene& s);

  // parsing helper functions
  int processList(Scene& s, SceneItem* parent, AstNode* node,
                  SceneItemFlag flag = NO_FLAG);

  int getBool(AstNode*& n, bool& val) const;
  int getString(AstNode*& n, std::string& val) const;
  int getFlt(AstNode*& n, Flt& val) const;
  int getInt(AstNode*& n, int& val) const;
  int getVec3(AstNode*& n, Vec3& v) const;

  std::string fileName(int file_id) const {
    auto itr = _files.find(file_id);
    return (itr == _files.end()) ? std::string("<unknown>") : itr->second;
  }

  template<typename... Args>
  void reportError(const AstNode* n, const Args&... args) const {
    println_err("[", fileName(n->file_id), ":", n->line, "] ", args...);
  }

 private:
  SList<AstNode> _astList;
  std::map<int,std::string> _files;
  int _lastID = 0;

  int processNode(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag);
};
