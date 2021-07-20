//
// Shader.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of shader module
//

#include "Shader.hh"
#include "Stats.hh"


// **** Shader Class ****
Shader::Shader()
{
  ++Inventory.shaders;
}

Shader::~Shader()
{
  --Inventory.shaders;
}


// **** PatternShader Class ****
// SceneItem Functions
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
    int err = InitShader(s, *sh, value, &_trans);
    if (err) { return err; }
  }

  _trans.init();
  return 0;
}


// **** Functions ****
int InitShader(Scene& s, Shader& sh, Flt value, const Transform* t)
{
  // Transform by parent trans
  Transform* trans = sh.trans();
  if (trans) {
    trans->global = trans->local;
    if (t) { trans->global *= t->global; }
  }

  // Scale value by parent value
  sh.value *= value;
  int error = sh.init(s);
  if (error) {
    return error;
  }

  sh.value = 1.0;
  return 0;
}
