//
// Light.cc
// Copyright (C) 2024 Richard Bradley
//

#include "Light.hh"
#include <cassert>


// **** Light Class ****
int Light::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_energy || flag != FLAG_NONE) { return -1; }

  _energy = sh;
  return 0;
}
