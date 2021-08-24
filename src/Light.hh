//
// Light.hh
// Copyright (C) 2021 Richard Bradley
//
// Light base class
//

#pragma once
#include "SceneItem.hh"
#include "Transform.hh"
#include "Color.hh"
#include "Types.hh"


// **** Types ****
class Scene;
class Shader;
class HitInfo;
class Ray;
struct EvaluatedHit;

struct LightResult {
  Vec3 dir;
  Flt distance;
  Color energy;
};

// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3 pos, dir;

  // SceneItem Functions
  Transform* trans() override final { return &_trans; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Member Functions
  virtual int init(Scene& s);
  virtual int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const = 0;

 protected:
  ShaderPtr _energy;
  Transform _trans;
};


// **** Functions ****
int InitLight(Scene& s, Light& lt, const Transform* t = nullptr);
