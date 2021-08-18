//
// Shader.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Shader.hh"
#include "Scene.hh"


// **** PatternShader Class ****
int PatternShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || flag != FLAG_NONE) { return -1; }

  _children.push_back(sh);
  return 0;
}

int PatternShader::init(Scene& s)
{
  if (_children.empty()) { return -1; }

  for (auto& sh : _children) {
    int err = InitShader(s, *sh, &_trans);
    if (err) { return err; }
  }

  _trans.init();
  return 0;
}


// **** Functions ****
int InitShader(Scene& s, Shader& sh, const Transform* t)
{
  // Transform by parent trans
  Transform* trans = sh.trans();
  if (trans) {
    trans->global = trans->local;
    if (t) { trans->global *= t->global; }
  }

  ++s.shader_count;
  return sh.init(s);
}
