//
// BasicLights.hh
// Copyright (C) 2021 Richard Bradley
//
// Basic light objects
//

#pragma once
#include "Light.hh"


// **** Types ****
class Sun : public Light
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<Sun>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};

class PointLight : public Light
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<PointLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};

class SpotLight : public Light
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<SpotLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, LightResult& result) const override;
};
