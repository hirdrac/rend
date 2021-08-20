//
// MapShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Default object map (EvaluatedHit.map value) uses the local intersection
// point as the map value.  These shaders override that.
//
// GlobalMapShader     - use global intersection point for map
// ConeMapShader       - cone/opencone surface map
// CubeMapShader       - cube surface map
// ParaboloidMapShader - paraboloid surface map
// SphereMapShader     - sphere surface map
// TorusMapShader      - torus surface map
//

#pragma once
#include "Shader.hh"
#include "Color.hh"


// **** MapShader base class ****
class MapShader : public Shader
{
 public:
  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s) override;

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
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


// **** Object Map Shaders ****
class MapConeShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCone>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class MapCubeShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCube>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class MapCylinderShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapCylinder>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class MapParaboloidShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapParaboloid>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class MapSphereShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapSphere>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class MapTorusShader final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<MapTorus>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};
