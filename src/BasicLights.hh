//
// BasicLights.hh
// Copyright (C) 2022 Richard Bradley
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
  int init(Scene& s) override;
  bool luminate(JobState& js, const Scene& s, const Ray& r,
                const EvaluatedHit& eh, LightResult& result) const override;

 private:
  Vec3 _finalDir;
};

class PointLight final : public Light
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<PointLight>"; }
  Transform* trans() override { return &_trans; }

  // Light Functions
  int init(Scene& s) override;
  bool luminate(JobState& js, const Scene& s, const Ray& r,
                const EvaluatedHit& eh, LightResult& result) const override;

 private:
  Transform _trans;
  Vec3 _finalPos;
};

class SpotLight final : public Light
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<SpotLight>"; }
  Transform* trans() override { return &_trans; }

  // Light Functions
  int init(Scene& s) override;
  bool luminate(JobState& js, const Scene& s, const Ray& r,
                const EvaluatedHit& eh, LightResult& result) const override;

 private:
  Transform _trans;
  Vec3 _finalPos, _finalDir;
};
