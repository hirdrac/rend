//
// RegisterFlag.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "Keywords.hh"
#include "Parser.hh"
#include "SceneItem.hh"


template<SceneItemFlag FLAG_VAL>
int FlagItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
               SceneItemFlag flag) {
  if (flag != FLAG_NONE) { return -1; }
  return sp.processList(s, p, n, FLAG_VAL);
}

#define REGISTER_FLAG_KEYWORD(flag,keyword)\
  static bool _flag_keyword_##flag = AddItemFn(keyword,&FlagItemFn<flag>)
