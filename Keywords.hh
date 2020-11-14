//
// Keywords.hh - revision 16 (2019/1/1)
// Copyright(C) 2019 by Richard Bradley
//

#ifndef Keywords_hh
#define Keywords_hh

#include "SceneItem.hh"
//#include <functional>


// **** Types ****
class AstNode;
class Scene;

//using ItemFn = std::function<int(Scene&,SceneItem*,AstNode*,SceneItemFlag)>;
typedef int (*ItemFn)
(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag);


// **** Functions ****
ItemFn FindItemFn(const std::string& str);

#endif
