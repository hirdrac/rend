//
// Occlusion.hh
// Copyright (C) 2021 Richard Bradley
//
// shader for ambient light
//

#pragma once
#include "Shader.hh"


class Occlusion final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Occlusion>"; }
  int setRadius(Flt v) override { _radius = v; return 0; }
  int setSamples(int v) override { _samples = v; return 0; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s, const Transform* tr) override;
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  ShaderPtr _child;
  Flt _radius = .1;
  int _samples = 4;
};
