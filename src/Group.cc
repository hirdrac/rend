//
// Group.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Group.hh"
#include "Scene.hh"


// **** Group Class ****
// SceneItem Functions
int Group::addObject(const ObjectPtr& ob)
{
  if (!ob) { return -1; }

  _children.push_back(ob);
  return 0;
}

// Object Functions
int Group::init(Scene& s)
{
  for (auto& ob : _children) {
    if (InitObject(s, *ob, _shader, &_trans)) { return -1; }
  }

  ++s.group_count;
  return 0;
}

int Group::intersect(const Ray& r, HitList& hit_list) const
{
  int hits = 0;
  for (auto& ob : _children) { hits += ob->intersect(r, hit_list); }
  return hits;
}
