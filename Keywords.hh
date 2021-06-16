//
// Keywords.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "SceneItem.hh"


// **** Types ****
class AstNode;
class Scene;

using ItemFn = int(*)(Scene& s, SceneItem* parent, AstNode* n, SceneItemFlag flag);


// **** Functions ****
[[nodiscard]] ItemFn FindItemFn(const std::string& str);
