//
// Light.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Light.hh"
#include "Scene.hh"
#include "Print.hh"
#include <cassert>


// **** Light Class ****
int Light::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_energy || flag != FLAG_NONE) { return -1; }

  _energy = sh;
  return 0;
}


// **** Functions ****
int InitLight(Scene& s, Light& lt, const Transform* tr)
{
  Transform* trans = lt.trans();
  if (trans) { trans->init(tr); }

  if (!lt.energy()) { lt.addShader(s.default_lt, FLAG_NONE); }

  return lt.init(s);
}
