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

  // Destructor
  ~Phong();

  // SceneItem Functions
  std::string desc(int) const override { return "<Phong>"; }
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  Shader* _diffuse = nullptr;
  Shader* _specular = nullptr;
  Shader* _transmit = nullptr;
};
