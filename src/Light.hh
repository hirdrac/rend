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
struct JobState;
class Scene;
class Ray;
struct EvaluatedHit;

struct LightResult {
  Vec3 dir;
  Flt distance;
  Flt angle;
  Color energy;
};

// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3 dir{0,0,1};

  // SceneItem Functions
  Transform* trans() override final { return &_trans; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override final;

  // Member Functions
  virtual int init(Scene& s) = 0;
  virtual bool luminate(JobState& js, const Scene& s, const Ray& r,
                        const EvaluatedHit& eh, LightResult& result) const = 0;

  const ShaderPtr& energy() const { return _energy; }

 protected:
  ShaderPtr _energy;
  Transform _trans;
};


// **** Functions ****
int InitLight(Scene& s, Light& lt, const Transform* tr);
