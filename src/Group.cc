//
// Group.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Group.hh"
#include "Scene.hh"
#include <cassert>


// **** Group Class ****
// SceneItem Functions
int Group::addObject(const ObjectPtr& ob)
{
  assert(ob != nullptr);
  _objects.push_back(ob);
  return 0;
}

int Group::addLight(const LightPtr& lt)
{
  assert(lt != nullptr);
  _lights.push_back(lt);
  return 0;
}

// Object Functions
int Group::init(Scene& s)
{
  for (auto& lt : _lights) {
    if (InitLight(s, *lt, &_trans)) { return -1; }
  }

  for (auto& ob : _objects) {
    if (InitObject(s, *ob, _shader, &_trans)) { return -1; }
  }

  ++s.group_count;
  return 0;
}

int Group::intersect(const Ray& r, HitList& hit_list) const
{
  int hits = 0;
  for (auto& ob : _objects) { hits += ob->intersect(r, hit_list); }
  return hits;
}
