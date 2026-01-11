//
// Keywords.hh
// Copyright (C) 2026 Richard Bradley
//

#pragma once
#include "SceneItem.hh"
#include "Types.hh"
#include <string_view>


// **** Types ****
using ItemFn = int(*)(SceneParser& sp, Scene& s, SceneItem* parent,
                      AstNode* n, SceneItemFlag flag);


// **** Functions ****
[[nodiscard]] ItemFn findItemFn(std::string_view keyword);
bool addItemFn(std::string_view keyword, ItemFn fn);
