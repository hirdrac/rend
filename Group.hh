//
// Group.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Object.hh"
#include "SList.hh"


// **** Types ****
// Group class
//  class for grouping together objects for transformations, etc
class Group final : public Primitive
{
 public:
  // Constructor
  Group();
  // Destructor
  ~Group();

  // SceneItem Functions
  std::string desc(int indent) const override;
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;

  // Object Functions
  int bound(BBox& b) const override { return -1; }
  int intersect(const Ray& r, HitList& hit_list) const override;
  Object* childList() const override { return child_list.head(); }

 protected:
  SList<Object> child_list;
};
