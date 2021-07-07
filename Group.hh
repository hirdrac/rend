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
  Group();
  ~Group();

  // SceneItem Functions
  std::string desc() const override;
  int setName(const std::string& str) override { _name = str; return 0; }
  int addObject(Object* ob) override;
  int init(Scene& s) override;

  // Object Functions
  int bound(BBox& b) const override { return -1; }
  int intersect(const Ray& r, HitList& hit_list) const override;
  const Object* childList() const override { return _childList.head(); }

 protected:
  std::string _name;
  SList<Object> _childList;
};
