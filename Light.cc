//
// Light.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Light.hh"
#include "Scene.hh"
#include "Stats.hh"


// **** Light Class ****
Light::Light()
{
  ++Inventory.lights;
}

Light::~Light()
{
  --Inventory.lights;
}

// Member Functions
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
  if (!sh || _energy || flag != NO_FLAG) { return -1; }

  _energy = sh;
  return 0;
}


// **** Functions ****
int InitLight(Scene& s, Light& lt, ShadowFn shadow_fn)
{
  if (!lt.shadow_fn) {
    lt.shadow_fn = shadow_fn;
  }

  int error = lt.init(s);
  if (error) {
    return error;
  }

  return 0;
}
