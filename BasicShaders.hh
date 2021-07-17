//
// BasicShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Various simple shaders
//

#pragma once
#include "Shader.hh"
#include "Color.hh"


// **** Types ****
class ShaderColor final : public Shader
{
 public:
  ShaderColor() { _color.clear(); }
  ShaderColor(const Color& c) : _color(c) { }
  ShaderColor(Flt r, Flt g, Flt b) { _color.setRGB(r,g,b); }

  // SceneItem Functions
  std::string desc() const override;

  // Shader Functions
  int init(Scene& s) override;
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;

 private:
  Color _color;
};


class ShaderGlobal final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Global>"; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s) override;
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;

 private:
  ShaderPtr _child;
};


class ShaderLocal final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Local>"; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  int init(Scene& s) override;
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;

 private:
  ShaderPtr _child;
};


class Checkerboard final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Checkerboard>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;
};


class ColorCube final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<ColorCube>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Ring>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;
};


class ShaderSide final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Side>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const override;
};
