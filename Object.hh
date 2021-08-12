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
#include "ShaderPtr.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
class HitInfo;
class HitList;
struct EvaluatedHit;
class Ray;
class BBox;


// Object base class
class Object : public SceneItem
{
 public:
  Object();
  ~Object();

  // SceneItem Functions
  virtual int setCost(Flt c) override { _cost = c; return 0; }

  // Member Functions
  virtual int init(Scene& s) { return 0; }
  virtual int bound(BBox& b) const { return -1; }
  virtual int intersect(const Ray& r, HitList& hit_list) const = 0;
  virtual int evalHit(const Ray& r, const HitInfo& h, EvaluatedHit& eh) const {
    return -1; }
  virtual Flt hitCost() const { return 0.0; }
  virtual const std::vector<ObjectPtr>& children() const { return _emptyList; }

  const ShaderPtr& shader() const { return _shader; }

 protected:
  ShaderPtr _shader;
  Flt _cost = -1.0; // non-negative to override default cost

 private:
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

 protected:
  Transform _trans;
};


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh,
               const Transform* t = nullptr);
