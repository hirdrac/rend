//
// Light.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Light.hh"
#include "Scene.hh"


// **** Light Class ****
int Light::init(Scene& s)
{
  if (!_energy) {
    _energy = s.default_lt;
    if (!_energy) {
      // FIXME - add error message here for missing shader
      return -1;
    }
  }

  return 0;
}

int Light::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || _energy || flag != FLAG_NONE) { return -1; }

  _energy = sh;
  return 0;
}


// **** Functions ****
int InitLight(Scene& s, Light& lt, const Transform* t)
{
  Transform* trans = lt.trans();
  if (trans) { trans->init(t); }

  return lt.init(s);
}
