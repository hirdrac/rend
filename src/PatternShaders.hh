//
// PatternShaders.hh
// Copyright (C) 2022 Richard Bradley
//
// Shaders that select sub-shaders based on a (transformable) repeating pattern
//
// 1D pattern shaders (use x map coord)
//   Stripe         - repeating vertical stripes
//
// 2D pattern shaders (use x,y map coords)
//   Checkerboard   - classic checkboard square pattern based on x,y map coords
//   Pinwheel       - sectors radiating outward w/ optional spin
//   Ring           - bands radiating from the origin
//   SquareRing     - square version for ring shader
//
// 3D pattern shaders (use x,y,z map coords)
//   Checkerboard3D - checkerboard pattern in 3 dimensions
//
// all pattern shaders take an optional border shader & borderwidth
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
  Transform* trans() override final { return &_trans; }
  int setBorderwidth(Flt bw) override final { _borderwidth = bw; return 0; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override final;

  // Shader Functions
  int init(Scene& s, const Transform* tr) override final;

 protected:
  Transform _trans;
  std::vector<ShaderPtr> _children;
  ShaderPtr _border;
  Flt _borderwidth = 0.05;

  [[nodiscard]] const Shader* child(int i) const {
    const int no = int(_children.size());
    int c = i % no;
    if (c < 0) { c += no; }
    return _children[std::size_t(c)].get();
  }
};


class Checkerboard final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Checkerboard>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class Checkerboard3D final : public PatternShader
{
  // SceneItem Functions
  std::string desc() const override { return "<Checkerboard3D>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};


class Pinwheel final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Pinwheel>"; }
  int setSectors(int v) override { _sectors = v; return 0; }
  int setSpin(Flt v) override { _spin = v; return 0; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  Flt _spin = 0;
  int _sectors = 6;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Ring>"; }
  int setOffset(Flt v) override { _offset = v; return 0; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  Flt _offset = 0.0;
};


class SquareRing final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<SquareRing>"; }
  int setOffset(Flt v) override { _offset = v; return 0; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  Flt _offset = 0.0;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;
};
