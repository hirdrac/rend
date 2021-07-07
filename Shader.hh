//
// Shader.hh
// Copyright (C) 2021 Richard Bradley
//
// Shader base classes
//

#pragma once
#include "Scene.hh"
#include "Transform.hh"
#include "Types.hh"
#include <vector>
#include <memory>


// **** Types ****
class HitInfo;
class Color;

class Shader : public SceneItem
{
 public:
  Flt value = 1.0;

  Shader();
  ~Shader();

  // Member Functions
  virtual int evaluate(
    const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
    const Vec3& map, Color& result) const = 0;
};

class PatternShader : public Shader
{
 public:
  // SceneItem Functions
  Transform*       trans() override       { return &_trans; }
  const Transform* trans() const override { return &_trans; }
  int init(Scene& s) override;
  int addShader(Shader* sh, SceneItemFlag flag) override;

 protected:
  Transform _trans;
  std::vector<std::unique_ptr<Shader>> _children;
};


// **** Functions ****
int InitShader(
  Scene& s, Shader& sh, Flt value = 1.0, const Transform* t = nullptr);
