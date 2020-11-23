//
// BasicShaders.hh
// Copyright (C) 2020 Richard Bradley
//
// Various simple shaders
//

#pragma once
#include "Shader.hh"
#include <iostream>


// **** Types ****
class ShaderColor : public Shader
{
 public:
  Color color;

  // Constructors
  ShaderColor() { color.clear(); }
  ShaderColor(const Color& c, Flt v = 1.0) : color(c) { }
  ShaderColor(Flt r, Flt g, Flt b) { color.setRGB(r,g,b); }

  // SceneItem Functions
  void print(std::ostream& out, int) const override {
    out << "<Color " << color << '>'; }
  int init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ShaderGlobal : public Shader
{
 public:
  // SceneItem Functions
  int  add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  void print(std::ostream& out, int) const override { out << "<Global>"; }
  int  init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  std::vector<Shader*> children;
};


class ShaderLocal : public Shader
{
 public:
  // SceneItem Functions
  int  add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  void print(std::ostream& out, int) const override { out << "<Local>"; }
  int  init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;

 private:
  std::vector<Shader*> children;
};


class Checkerboard : public PatternShader
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Checkerboard>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ColorCube : public PatternShader
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<ColorCube>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Ring : public PatternShader
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Ring>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class ShaderSide : public PatternShader
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Side>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};


class Stripe : public PatternShader
{
 public:
  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Stripe>"; }

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};

#if 0
class TextureMap : public PatternShader
{
 public:
  FrameBuffer* fb;
  Flt  sx, sy;

  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<TextureMap>"; }
  int  init(Scene& s) override;

  // Shader Functions
  int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const override;
};
#endif
