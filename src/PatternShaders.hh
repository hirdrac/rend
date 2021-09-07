//
// PatternShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Various simple shaders
//

#pragma once
#include "Shader.hh"
#include "ShaderPtr.hh"
#include "Transform.hh"
#include <vector>


// **** Types ****
class PatternShader : public Shader
{
 public:
  // SceneItem Functions
  Transform* trans() override { return &_trans; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s) override;

 protected:
  Transform _trans;
  std::vector<ShaderPtr> _children;
};


class Checkerboard final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Checkerboard>"; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Ring>"; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;
};


class ShaderSide final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Side>"; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;
};
