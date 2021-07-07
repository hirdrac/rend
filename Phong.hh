//
// Phong.hh
// Copyright (C) 2021 Richard Bradley
//
// Standard phong illumination model shader
//

#pragma once
#include "Shader.hh"
#include <memory>


// **** Types ****
class Phong final : public Shader
{
 public:
  Flt exp = 5.0;

  // SceneItem Functions
  std::string desc() const override { return "<Phong>"; }
  int addShader(Shader* sh, SceneItemFlag flag) override;
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  std::unique_ptr<Shader> _diffuse, _specular, _transmit;
};
