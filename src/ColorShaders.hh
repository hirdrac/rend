//
// ColorShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Color generating shaders
//

#pragma once
#include "Shader.hh"
#include "Transform.hh"


// **** Types ****
class ColorCube final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<ColorCube>"; }
  Transform* trans() override { return &_trans; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;

 private:
  Transform _trans;
};
