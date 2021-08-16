//
// BasicLights.hh
// Copyright (C) 2021 Richard Bradley
//
// Basic light objects
//

#pragma once
#include "Light.hh"


// **** Types ****
class Sun final : public Light
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Sun>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const override;
};

class PointLight final : public Light
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<PointLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const override;
};

class SpotLight final : public Light
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<SpotLight>"; }

  // Light Functions
  int luminate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    LightResult& result) const override;
};
