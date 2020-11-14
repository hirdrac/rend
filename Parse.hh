//
// Parse.hh - revision 20 (2019/1/9)
// Copyright(C) 2019 by Richard Bradley
//
// Scene description file parser
//

#ifndef Parse_hh
#define Parse_hh

#include "SceneItem.hh"
#include "Types.hh"
#include "SList.hh"
#include <string>


// **** Types ****
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
  void print(std::ostream& stream, int indent = 0) const;
};


class SceneDesc
{
 public:
  SList<AstNode> node_list;

  // Member Functions
  int parseFile(const std::string& file);
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

#endif
