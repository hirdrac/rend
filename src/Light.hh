//
// Light.hh
// Copyright (C) 2025 Richard Bradley
//
// Light base class
//

#pragma once
#include "SceneItem.hh"
#include "Transform.hh"
#include "Color.hh"
#include "Types.hh"


// **** Types ****
struct LightResult {
  Vec3 dir{INIT_NONE};
  Flt distance;
  Flt angle;
  Color energy{INIT_NONE};
};

// Light Base Class
class Light : public SceneItem
{
 public:
  Vec3 dir{0,0,1};

  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) final;

  // Member Functions
  virtual int init(Scene& s) = 0;
  virtual bool luminate(JobState& js, const Scene& s, const Ray& r,
                        const EvaluatedHit& eh, LightResult& result) const = 0;

  [[nodiscard]] const ShaderPtr& energy() const { return _energy; }

 protected:
  ShaderPtr _energy;
};
