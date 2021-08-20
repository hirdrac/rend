//
// MapShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Generate texture coordinates for objects
// (EvaluatedHit.map values)
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


// **** Shaders ****
class ShaderGlobal final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Global>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class ShaderLocal final : public MapShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Local>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};
