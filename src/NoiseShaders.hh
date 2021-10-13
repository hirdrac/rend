//
// NoiseShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// NoiseShader - adds noise to x map coordinate
//   * 'value' setting to scale noise amount
//   * transform to changing input map coord
//

#pragma once
#include "Shader.hh"
#include "ShaderPtr.hh"
#include "Transform.hh"


class NoiseShader final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<NoiseShader>"; }
  Transform* trans() override { return &_trans; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;
  int setValue(Flt v) override { _value = v; return 0; }

  // Shader Functions
  int init(Scene& s, const Transform* tr) override;
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 protected:
  Transform _trans;
  ShaderPtr _child;
  Flt _value = 1.0;
};
