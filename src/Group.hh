//
// Group.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Object.hh"
#include "Light.hh"


// **** Types ****
// Group class
//  class for grouping together objects for transformations, etc
class Group final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Group>"; }
  int addObject(const ObjectPtr& ob) override;
  int addLight(const LightPtr& lt) override;

  // Object Functions
  int init(Scene& s) override;
  int bound(BBox& b) const override { return -1; }
  int intersect(const Ray& r, HitList& hit_list) const override;
  const std::vector<ObjectPtr>& children() const override { return _objects; }

 protected:
  std::vector<ObjectPtr> _objects;
  std::vector<LightPtr> _lights;
};
