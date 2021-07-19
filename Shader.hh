//
// Shader.hh
// Copyright (C) 2021 Richard Bradley
//
// Shader base classes
//

#pragma once
#include "SceneItem.hh"
#include "Transform.hh"
#include "ShaderPtr.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
class HitInfo;
struct EvaluatedHit;
class Color;
class Ray;

class Shader : public SceneItem
{
 public:
  Flt value = 1.0;

  Shader();
  ~Shader();

  // Member Functions
  virtual int init(Scene& s) { return 0; }
  virtual int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
    Color& result) const = 0;
};

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


// **** Functions ****
int InitShader(Scene& s, Shader& sh, Flt value = 1.0,
               const Transform* t = nullptr);
