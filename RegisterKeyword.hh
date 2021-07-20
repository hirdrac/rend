//
// RegisterKeyword.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Keywords.hh"
#include "Parser.hh"
#include "ObjectPtr.hh"
#include "LightPtr.hh"
#include "ShaderPtr.hh"
#include "Shader.hh"
#include "Scene.hh"


template<class ObjectType>
int ObjectItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                 SceneItemFlag) {
  auto ob = makeObject<ObjectType>();
  int error = p ? p->addObject(ob) : s.addObject(ob);
  return error ? error : sp.processList(s, ob.get(), n);
}

template<class LightType>
int LightItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                SceneItemFlag) {
  auto lt = makeLight<LightType>();
  int error = p ? p->addLight(lt) : s.addLight(lt);
  return error ? error : sp.processList(s, lt.get(), n);
}

template<class ShaderType>
int ShaderItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                 SceneItemFlag flag) {
  auto sh = makeShader<ShaderType>();
  int error = 0;
  if (!p) { error = s.addShader(sh, flag); } else {
    error = p->addShader(sh, flag);
    if (!error && !dynamic_cast<Shader*>(p)) {
      error = s.addShader(sh, NO_FLAG);  // add to scene for init
    }
  }
  return error ? error : sp.processList(s, sh.get(), n);
}


#define REGISTER_OBJECT_KEYWORD(type,keyword)\
  static bool _object_keyword_##type = AddItemFn(keyword,&ObjectItemFn<type>)
#define REGISTER_LIGHT_KEYWORD(type,keyword)\
  static bool _light_keyword_##type = AddItemFn(keyword,&LightItemFn<type>)
#define REGISTER_SHADER_KEYWORD(type,keyword)\
  static bool _shader_keyword_##type = AddItemFn(keyword,&ShaderItemFn<type>)
