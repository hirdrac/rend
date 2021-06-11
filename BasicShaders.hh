//
// BasicShaders.hh
// Copyright (C) 2021 Richard Bradley
//
// Various simple shaders
//

#pragma once
#include "Shader.hh"


// **** Types ****
class ShaderColor final : public Shader
{
 public:
  Color color;

  // Constructors
  ShaderColor() { color.clear(); }
  ShaderColor(const Color& c, Flt v = 1.0) : color(c) { }
  ShaderColor(Flt r, Flt g, Flt b) { color.setRGB(r,g,b); }

  // SceneItem Functions
  std::string desc(int) const override;
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ShaderGlobal final : public Shader
{
 public:
  // SceneItem Functions
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  std::string desc(int) const override { return "<Global>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  Shader* child = nullptr;
};


class ShaderLocal final : public Shader
{
 public:
  // SceneItem Functions
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  std::string desc(int) const override { return "<Local>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  Shader* child = nullptr;
};


class Checkerboard final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<Checkerboard>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ColorCube final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<ColorCube>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Ring final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<Ring>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ShaderSide final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<Side>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Stripe final : public PatternShader
{
 public:
  // SceneItem Functions
  std::string desc(int) const override { return "<Stripe>"; }

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
  std::string desc(int) const override { return "<TextureMap>"; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};
#endif
