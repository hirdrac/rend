//
// Bound.hh
// Copyright (C) 2023 Richard Bradley
//
// Definition of Bound object class and
// bounding box hierarchy function
//

#pragma once
#include "Object.hh"
#include "BBox.hh"
#include <vector>


// **** Types ****
class Bound final : public Object
{
 public:
  std::vector<ObjectPtr> objects;
  BBox box; // bound size

  // SceneItem Functions
  std::string desc() const override;

  // Object Functions
  BBox bound(const Matrix* t) const override { return box; }
  int intersect(const Ray& r, HitList& hl) const override;
  std::span<const ObjectPtr> children() const override { return objects; }
};


// **** Functions ****
int MakeBoundList(const Scene& s, std::span<const ObjectPtr> o_list,
                  std::vector<ObjectPtr>& bound_list);
