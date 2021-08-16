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
  ShaderColor() : _color{colors::black} { }
  ShaderColor(const Color& c) : _color{c} { }
  ShaderColor(Color::value_type r, Color::value_type g, Color::value_type b)
    : _color{r,g,b} { }

  // SceneItem Functions
  std::string desc() const override;

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;

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
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;

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
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;

 private:
  ShaderPtr _child;
};


class Checkerboard final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Checkerboard>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class ColorCube final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<ColorCube>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Ring>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class ShaderSide final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Side>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                 const EvaluatedHit& eh) const override;
};
