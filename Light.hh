//
// Light.hh
// Copyright (C) 2021 Richard Bradley
//
// Light base class
//

#pragma once
#include "SceneItem.hh"
#include "Color.hh"
#include "Shadow.hh"
#include "Types.hh"


// **** Types ****
class Shader;
class HitInfo;
class EvaluatedHit;


struct LightResult {
  Color energy;
  Vec3 dir;
  Flt distance;
};


// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3      pos, dir;
  ShaderPtr energy;
  ShadowFn  shadow_fn = nullptr;

  Light();
  ~Light();

  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;
  int init(Scene& s) override final;

  // Member Functions
  virtual int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const = 0;
};


// **** Functions ****
int InitLight(Scene& s, Light& lt, ShadowFn shadow_fn);
