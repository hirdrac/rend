//
// Phong.hh
// Copyright (C) 2021 Richard Bradley
//
// Standard phong illumination model shader
//

#pragma once
#include "Shader.hh"


// **** Types ****
class Phong final : public Shader
{
 public:
  Flt exp = 5.0;

  // SceneItem Functions
  std::string desc() const override { return "<Phong>"; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s, const Transform* tr) override;
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  ShaderPtr _ambient, _diffuse, _specular, _transmit;
};
