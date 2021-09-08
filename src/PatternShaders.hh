//
// PatternShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Shaders that select sub-shaders based on a (transformable) repeating pattern
//
// Checkerboard - classic checkboard square pattern
// Ring         - bands radiating from the origin
// Stripe       - repeating vertical stripes
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
  int setBorderwidth(Flt bw) override { _borderwidth = bw; return 0; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s) override;

 protected:
  Transform _trans;
  std::vector<ShaderPtr> _children;
  ShaderPtr _border;
  Flt _borderwidth = 0.05;
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


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  Color evaluate(
    const Scene& s, const Ray& r, const EvaluatedHit& eh) const override;
};
