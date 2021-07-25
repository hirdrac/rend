//
// Bound.hh
// Copyright (C) 2021 Richard Bradley
//
// Defintion of Bound object class and
// bounding box hierarchy function
//

#pragma once
#include "Object.hh"
#include "HitCostInfo.hh"
#include "BBox.hh"
#include <vector>


// **** Types ****
class Bound final : public Object
{
 public:
  std::vector<ObjectPtr> objects;
  BBox box; // bound size
  bool always_hit = false;

  Bound();
  ~Bound();

  // SceneItem Functions
  std::string desc() const override;

  // Object Functions
  int intersect(const Ray& r, bool csg, HitList& hit_list) const override;
  Flt hitCost() const override { return CostTable.bound; }
  const std::vector<ObjectPtr>& children() const override { return objects; }
};


// **** Functions ****
ObjectPtr MakeBoundList(const Vec3& eye, const std::vector<ObjectPtr>& o_list);
