//
// Light.cc - revision 21 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//

#include "Light.hh"
#include "Scene.hh"
#include "Stats.hh"
#include <iostream>


/**** Light Class ****/
// Constructor
Light::Light()
{
  ++Inventory.lights;
}

// Destructor
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
  if (sh) {
    energy = sh;
    return 0;
  } else {
    return -1;
  }
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
