//
// CSG.hh
// Copyright (C) 2021 Richard Bradley
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
  // SceneItem Functions
  int addObject(const ObjectPtr& ob) override;

  // Object Functions
  int init(Scene& s) override;
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;
  const std::vector<ObjectPtr>& children() const override { return _children; }

 protected:
  std::vector<ObjectPtr> _children;
};


// **** CSG classes ****
class Merge final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Merge>"; }

  // Object Functions
  BBox bound() const override;
  BBox localBound() const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
};

class Union final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Union>"; }

  // Object Functions
  BBox bound() const override;
  BBox localBound() const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
};

class Intersection final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Intersection>"; }

  // Object Functions
  BBox bound() const override;
  BBox localBound() const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
};

class Difference final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Difference>"; }

  // Object Functions
  BBox bound() const override;
  BBox localBound() const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
};
