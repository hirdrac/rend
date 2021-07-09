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
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  Color _color;
};


class ShaderGlobal final : public Shader
{
 public:
  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;
  std::string desc() const override { return "<Global>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  ShaderPtr _child;
};


class ShaderLocal final : public Shader
{
 public:
  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;
  std::string desc() const override { return "<Local>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

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
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ColorCube final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<ColorCube>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Ring>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ShaderSide final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Side>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Stripe>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};

#if 0
class TextureMap final : public PatternShader
{
 public:
  FrameBuffer* fb;
  Flt sx, sy;

  // SceneItem Functions
  std::string desc() const override { return "<TextureMap>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};
#endif
