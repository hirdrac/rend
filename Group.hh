//
// Group.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Object.hh"


// **** Types ****
// Group class
//  class for grouping together objects for transformations, etc
class Group final : public Primitive
{
 public:
  Group();
  ~Group();

  // SceneItem Functions
  std::string desc() const override;
  int setName(const std::string& str) override { _name = str; return 0; }
  int addObject(const ObjectPtr& ob) override;

  // Object Functions
  int init(Scene& s) override;
  int bound(BBox& b) const override { return -1; }
  int intersect(const Ray& r, bool csg, HitList& hit_list) const override;
  const std::vector<ObjectPtr>& children() const override { return _children; }

 protected:
  std::string _name;
  std::vector<ObjectPtr> _children;
};
