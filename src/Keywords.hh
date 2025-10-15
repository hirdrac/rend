//
// Keywords.hh
// Copyright (C) 2025 Richard Bradley
//

#pragma once
#include "SceneItem.hh"
#include "Types.hh"
#include <string_view>


// **** Types ****
using ItemFn = int(*)(SceneParser& sp, Scene& s, SceneItem* parent,
                      AstNode* n, SceneItemFlag flag);


// **** Functions ****
[[nodiscard]] ItemFn FindItemFn(std::string_view keyword);
bool AddItemFn(std::string_view keyword, ItemFn fn);
