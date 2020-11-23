//
// Phong.hh
// Copyright (C) 2020 Richard Bradley
//
// Standard phong illumination model shader
//

#pragma once
#include "Shader.hh"


// **** Types ****
class Phong : public Shader
{
 public:
  Shader* diffuse = nullptr;
  Shader* specular = nullptr;
  Shader* transmit = nullptr;
  Flt     exp = 5.0;

  // Destructor
  ~Phong();

  // SceneItem Functions
  void print(std::ostream& stream, int) const override { stream << "<Phong>"; }
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};
