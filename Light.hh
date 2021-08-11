//
// Light.hh
// Copyright (C) 2021 Richard Bradley
//
// Light base class
//

#pragma once
#include "SceneItem.hh"
#include "Color.hh"
#include "Types.hh"


// **** Types ****
class Scene;
class Shader;
class HitInfo;
class Ray;
struct EvaluatedHit;

struct LightResult {
  Color energy;
  Vec3 dir;
  Flt distance;
};

// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3 pos, dir;

  Light();
  ~Light();

  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Member Functions
  int init(Scene& s);
  virtual int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const = 0;

 protected:
  ShaderPtr _energy;
};


// **** Functions ****
int InitLight(Scene& s, Light& lt);
