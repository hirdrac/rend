//
// Shader.hh - revision 29 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//
// Shader base classes
//

#ifndef Shader_hh
#define Shader_hh

#include "Scene.hh"
#include "Transform.hh"
#include "Types.hh"
#include <iostream>
#include <vector>


// **** Types ****
class HitInfo;
class Color;

class Shader : public SceneItem
{
 public:
  Flt value = 1.0;

  // Constructor
  Shader();
  // Destructor
  ~Shader();

  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Shader>"; }

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
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;

 protected:
  Transform _trans;
  std::vector<Shader*> children;
};


// **** Functions ****
int InitShader(
  Scene& s, Shader& sh, Flt value = 1.0, const Transform* t = nullptr);

#endif
