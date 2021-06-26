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
#include <string>


// **** Types ****
class Scene;

enum AstType {
  AST_UNKNOWN, AST_LIST, AST_NUMBER, AST_KEYWORD
};


class AstNode : public SListNode<AstNode>
{
 public:
  AstNode*    child = nullptr;
  AstType     ast_type = AST_UNKNOWN;
  int         line = 0;
  std::string val;
  void*       ptr = nullptr;
  int         flag = 0;

  // Constructors
  AstNode() = default;
  AstNode(int fileLine) : line(fileLine) { }
  AstNode(int fileLine, std::string value)
    : line(fileLine), val(std::move(value)) { }

  // Member Functions
  std::string desc(int indent) const;
  void setType();
};


class SceneDesc
{
 public:
  // Member Functions
  int parseFile(const std::string& file);
  int setupScene(Scene& s) const;

 private:
  SList<AstNode> _astList;
};


// **** Functions ****
int ProcessList(Scene& s, SceneItem* parent, AstNode* n,
		SceneItemFlag flag = NO_FLAG);

int GetString(AstNode*& n, std::string& val, const std::string& def);
int GetBool(AstNode*& n, bool& val, bool def);
int GetFlt(AstNode*& n, Flt& val, Flt def);
int GetInt(AstNode*& n, int& val, int def);
int GetVec2(AstNode*& n, Vec2& v, const Vec2& def);
int GetVec3(AstNode*& n, Vec3& v, const Vec3& def);
