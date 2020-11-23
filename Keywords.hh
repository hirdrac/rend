//
// Keywords.hh
// Copyright (C) 2020 Richard Bradley
//

#pragma once
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
