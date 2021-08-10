//
// Bound.hh
// Copyright (C) 2021 Richard Bradley
//
// Defintion of Bound object class and
// bounding box hierarchy function
//

#pragma once
#include "Object.hh"
#include "BBox.hh"
#include <vector>
#include <memory>


// **** Types ****
class Bound final : public Object
{
 public:
  std::vector<ObjectPtr> objects;
  BBox box; // bound size

  Bound();
  ~Bound();

  // SceneItem Functions
  std::string desc() const override;

  // Object Functions
  int intersect(const Ray& r, bool csg, HitList& hit_list) const override;
  const std::vector<ObjectPtr>& children() const override { return objects; }
};

using BoundPtr = std::shared_ptr<Bound>;


// **** Functions ****
BoundPtr MakeBoundList(const Vec3& eye, const std::vector<ObjectPtr>& o_list);
