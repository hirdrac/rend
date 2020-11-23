//
// BasicLights.hh
// Copyright (C) 2020 Richard Bradley
//
// Basic light objects
//

#pragma once
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
