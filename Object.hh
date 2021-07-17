//
// Object.hh
// Copyright (C) 2021 Richard Bradley
//
// Definition of base object class
//

#pragma once
#include "SceneItem.hh"
#include "Transform.hh"
#include "ObjectPtr.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
class HitInfo;
class HitList;
class Shader;
class Ray;
class BBox;


// Object base class
class Object : public SceneItem
{
 public:
  Object();
  ~Object();

  // Member Functions
  virtual int init(Scene& s) { return 0; }
  virtual int bound(BBox& b) const { return -1; }
  virtual int intersect(const Ray& r, bool csg, HitList& hit_list) const = 0;
  virtual int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const {
    return -1; }
  virtual Flt hitCost() const { return 1.0; }
  virtual const ShaderPtr& shader() const { return _nullShader; }
  virtual int setShader(const ShaderPtr& sh) { return -1; }
  virtual const std::vector<ObjectPtr>& children() const { return _emptyList; }

 private:
  static const ShaderPtr _nullShader;
  static const std::vector<ObjectPtr> _emptyList;
};


// Primitive base class
//   a visible scene object (sphere, csg, etc.) that can be transformed
class Primitive : public Object
{
 public:
  // SceneItem Functions
  Transform* trans() override { return &_trans; }
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Object Functions
  int init(Scene& s) override;
  int bound(BBox& b) const override;
  const ShaderPtr& shader() const override { return _shader; }
  int setShader(const ShaderPtr& sh) override { _shader = sh; return 0; }

 protected:
  Transform _trans;
  ShaderPtr _shader;
};


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh,
               const Transform* t = nullptr);
