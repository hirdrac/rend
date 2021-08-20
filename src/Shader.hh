//
// Shader.hh
// Copyright (C) 2021 Richard Bradley
//
// Shader base class
//

#pragma once
#include "SceneItem.hh"
#include "ShaderPtr.hh"
#include "Color.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
class HitInfo;
struct EvaluatedHit;
class Color;
class Ray;
class Transform;

class Shader : public SceneItem
{
 public:
  // Member Functions
  virtual int init(Scene& s) { return 0; }
  virtual Color evaluate(const Scene& s, const Ray& r, const HitInfo& h,
                         const EvaluatedHit& eh) const = 0;
};

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


// **** Functions ****
int InitShader(Scene& s, Shader& sh, const Transform* t = nullptr);
