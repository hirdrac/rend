//
// Shader.hh
// Copyright (C) 2024 Richard Bradley
//
// Shader base class & simple shaders:
//
// ShaderColor - solid color value
// ShaderSide  - select shader based on intersection side
//

#pragma once
#include "ShaderPtr.hh"
#include "SceneItem.hh"
#include "Color.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
struct JobState;
class Ray;
class Transform;

struct EvaluatedHit {
  Vec3 global_pt{INIT_NONE};
  Vec3 normal{INIT_NONE};
  Vec3 map{INIT_NONE};
  int side;
};

class Shader : public SceneItem
{
 public:
  // Member Functions
  virtual int init(Scene& s, const Transform* tr) { return 0; }
  virtual Color evaluate(JobState& js, const Scene& s, const Ray& r,
                         const EvaluatedHit& eh) const = 0;
};


// Simple General Shaders
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
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  Color _color;
};

class ShaderSide final : public Shader
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Side>"; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Shader Functions
  Color evaluate(JobState& js, const Scene& s, const Ray& r,
                 const EvaluatedHit& eh) const override;

 private:
  std::vector<ShaderPtr> _sideShaders;
};
