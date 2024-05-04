//
// Object.hh
// Copyright (C) 2024 Richard Bradley
//
// Definition of base object class
//

#pragma once
#include "SceneItem.hh"
#include "Transform.hh"
#include "ObjectPtr.hh"
#include "ShaderPtr.hh"
#include "Types.hh"
#include <span>


// **** Types ****
class HitInfo;
class HitList;
class HitCostInfo;
class Ray;
class BBox;


// Object base class
class Object : public SceneItem
{
 public:
  // SceneItem Functions
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) final;

  // Member Functions
  virtual int init(Scene& s, const Transform* tr) { return 0; }
  virtual BBox bound(const Matrix* t = nullptr) const = 0;
  virtual int intersect(const Ray& r, HitList& hl) const = 0;
  virtual std::span<const ObjectPtr> children() const { return {}; }

  [[nodiscard]] const ShaderPtr& shader() const { return _shader; }

 protected:
  ShaderPtr _shader;
};


// Primitive base class
//   a visible scene object (sphere, csg, etc.) that can be transformed
class Primitive : public Object
{
 public:
  // SceneItem Functions
  Transform* trans() final { return &_trans; }
  int setCost(Flt c) final { _cost = c; return 0; }

  // Object Functions
  BBox bound(const Matrix* t = nullptr) const override;

  // Member Functions
  virtual Flt hitCost(const HitCostInfo& hc) const = 0;
  virtual Vec3 normal(const Ray& r, const HitInfo& h) const = 0;

 protected:
  Transform _trans;
  Flt _cost = -1.0; // non-negative to override default cost
};
