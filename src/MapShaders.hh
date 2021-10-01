//
// MapShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Default object map (EvaluatedHit.map value) uses the local intersection
// point as the map value.  These shaders override that.
//
// MapGlobalShader     - use global intersection point for map
// MapConeShader       - cone/opencone surface map
// MapCubeShader       - cube surface map
// MapParaboloidShader - paraboloid surface map
// MapSphereShader     - sphere surface map
// MapTorusShader      - torus surface map
//

#pragma once
#include "Shader.hh"


// **** MapShader base class ****
class MapShader : public Shader
{
 public:
  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s, const Transform* tr) override;

 protected:
  ShaderPtr _child;
};


// **** General Map Shaders ****
class MapGlobalShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapGlobal>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


// **** Object Map Shaders ****
class MapConeShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCone>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class MapCubeShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCube>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class MapCylinderShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCylinder>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class MapParaboloidShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapParaboloid>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class MapSphereShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapSphere>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class MapTorusShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapTorus>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};
