//
// Shader.cc - revision 30 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//
// Implementation of shader module
//

#include "Shader.hh"
#include "Stats.hh"


// **** Shader Class ****
// Constructor
Shader::Shader()
{
  ++Inventory.shaders;
}

// Destructor
Shader::~Shader()
{
  --Inventory.shaders;
}


// **** PatternShader Class ****
// SceneItem Functions
int PatternShader::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh) { return -1; }

  children.push_back(sh);
  return 0;
}

int PatternShader::init(Scene& s)
{
  if (children.empty()) { return -1; }

  for (Shader* sh : children) {
    int err = InitShader(s, *sh, value, &_trans);
    if (err) { return err; }
  }

  Shader::init(s);
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
