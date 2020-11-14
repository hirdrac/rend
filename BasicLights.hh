//
// BasicLights.hh - revision 1 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//
// Basic light objects
//

#ifndef BasicLight_hh
#define BasicLight_hh

#include "Light.hh"
#include <iostream>


// **** Types ****
class Sun : public Light
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Sun>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};

class PointLight : public Light
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<PointLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};

class SpotLight : public Light
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<SpotLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};

#endif
