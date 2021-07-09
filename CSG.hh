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
  CSG();
  ~CSG();

  // SceneItem Functions
  int addObject(const ObjectPtr& ob) override;
  int init(Scene& s) override;

  // Object Functions
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override;
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
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Union final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Union>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Intersection final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Intersection>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Difference final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Difference>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};
