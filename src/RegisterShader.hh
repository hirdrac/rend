//
// RegisterShader.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "Keywords.hh"
#include "Parser.hh"
#include "ShaderPtr.hh"
#include "Shader.hh"
#include "Scene.hh"


template<class ShaderType>
int ShaderItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                 SceneItemFlag flag) {
  auto sh = makeShader<ShaderType>();
  int error = 0;
  if (!p) { error = s.addShader(sh, flag); } else {
    error = p->addShader(sh, flag);
    if (!error && !dynamic_cast<Shader*>(p)) {
      error = s.addShader(sh, FLAG_INIT_ONLY); // add to scene for init
    }
  }
  return error ? error : sp.processList(s, sh.get(), n);
}

#define REGISTER_SHADER_CLASS(type,keyword)\
  static bool _shader_keyword_##type = AddItemFn(keyword,&ShaderItemFn<type>)
