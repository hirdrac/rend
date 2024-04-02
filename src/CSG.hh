//
// CSG.hh
// Copyright (C) 2024 Richard Bradley
//
// CSG object classes
//

#pragma once
#include "Object.hh"
#include <vector>


// **** Class Definitions ****
class CSG : public Primitive
{
 public:
  std::vector<ObjectPtr> objects;

  // SceneItem Functions
  int addObject(const ObjectPtr& ob) final;

  // Object Functions
  int init(Scene& s, const Transform* tr) final;
  std::span<const ObjectPtr> children() const final {
    return objects; }

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const final;
  Vec3 normal(const Ray& r, const HitInfo& h) const final { return {}; }
};


// **** CSG classes ****
class Union final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Union>"; }

  // Object Functions
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;
};

class Intersection final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Intersection>"; }

  // Object Functions
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;
};

class Difference final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Difference>"; }

  // Object Functions
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;
};
