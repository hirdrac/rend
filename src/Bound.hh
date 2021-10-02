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


// **** Types ****
class Bound final : public Object
{
 public:
  std::vector<ObjectPtr> objects;
  BBox box; // bound size

  // SceneItem Functions
  std::string desc() const override;

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  BBox bound(const Matrix* t) const override { return box; }
  int intersect(const Ray& r, HitList& hl) const override;
  const std::vector<ObjectPtr>& children() const override { return objects; }
};


// **** Functions ****
int MakeBoundList(const Scene& s, const std::vector<ObjectPtr>& o_list,
                  std::vector<ObjectPtr>& bound_list);
