//
// Group.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Object.hh"
#include "LightPtr.hh"
#include "Transform.hh"


// **** Types ****
// Group class
//  class for grouping together objects for transformations, etc
class Group final : public Object
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Group>"; }
  Transform* trans() override { return &_trans;}
  int addObject(const ObjectPtr& ob) override;
  int addLight(const LightPtr& lt) override;
  int addShader(const ShaderPtr& sh, SceneItemFlag flag) override;

  // Object Functions
  int init(Scene& s) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
  const std::vector<ObjectPtr>& children() const override { return _objects; }

 protected:
  Transform _trans;
  std::vector<ObjectPtr> _objects;
  std::vector<LightPtr> _lights;
};
