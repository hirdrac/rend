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

// Bounding Box class definition
//  simple class for storing object bounding box information
class BBox
{
 public:
  Vec3 pmin, pmax;

  BBox() { reset(); }
  BBox(const BBox& b1, const BBox& b2);

  // Member Functions
  void reset();
  [[nodiscard]] Flt weight() const;

  void fit(const Vec3& pt);
  void fit(const BBox& box);
  void fit(const Transform& t, const Vec3* pt_list, int pt_count);

  [[nodiscard]] bool empty() const {
    return IsGreater(pmin.x, pmax.x) || IsGreater(pmin.y, pmax.y)
      || IsGreater(pmin.z, pmax.z); }
};


// Object base class
class Object : public SceneItem
{
 public:
  Object();
  ~Object();

  // Member Functions
  virtual int bound(BBox& b) const { return -1; }
  virtual int intersect(const Ray& r, bool csg, HitList& hit_list) const = 0;
  virtual int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const {
    return -1; }
  virtual Flt hitCost() const { return 1.0; }
  virtual int setRadius(Flt r) { return -1; }
  virtual bool isVisible() const { return false; }
  virtual bool isSolid() const { return false; }
  virtual int setSolid(bool s) { return -1; }
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
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;
  int init(Scene& s) override;
  Transform*       trans() override       { return &_trans; }
  const Transform* trans() const override { return &_trans; }

  // Object Functions
  int bound(BBox& b) const override;
  bool isVisible() const override    { return true; }
  bool isSolid() const override      { return _solid; }
  int setSolid(bool s) override      { _solid = s; return 0; }
  const ShaderPtr& shader() const override { return _shader; }
  int setShader(const ShaderPtr& sh) override { _shader = sh; return 0; }

 protected:
  Transform _trans;
  ShaderPtr _shader;
  bool _solid = false;
};


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh,
               const Transform* t = nullptr);
