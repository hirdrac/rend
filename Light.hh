//
// Light.hh
// Copyright (C) 2020 Richard Bradley
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


struct LightResult {
  Color energy;
  Vec3 dir;
  Flt distance;
};


// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3     pos, dir;
  Shader*  energy = nullptr; // not owner
  ShadowFn shadow_fn = nullptr;

  // Constructor
  Light();
  // Destructor
  ~Light();

  // SceneItem Functions
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override final;

  // Member Functions
  virtual int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const = 0;
};


// **** Functions ****
int InitLight(Scene& s, Light& lt, ShadowFn shadow_fn);