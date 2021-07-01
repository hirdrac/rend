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
  if (!energy) {
    energy = s.default_lt;
  }

  return 0;
}

int Light::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh || energy) {
    return -1;
  }

  energy = sh;
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
