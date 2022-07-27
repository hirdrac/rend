//
// RegisterLight.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "Keywords.hh"
#include "Parser.hh"
#include "LightPtr.hh"
#include "Scene.hh"


template<class LightType>
int LightItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                SceneItemFlag) {
  auto lt = makeLight<LightType>();
  int error = s.addLight(lt);
  if (!error && p) { error = p->addLight(lt); }
  return error ? error : sp.processList(s, lt.get(), n);
}

#define REGISTER_LIGHT_CLASS(type,keyword)\
  static bool _light_keyword_##type = AddItemFn(keyword,&LightItemFn<type>)
