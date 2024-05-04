//
// Group.cc
// Copyright (C) 2024 Richard Bradley
//

#include "Group.hh"
#include "Scene.hh"
#include "BBox.hh"
#include "Light.hh"
#include "RegisterObject.hh"
#include <cassert>


// **** Group Class ****
REGISTER_OBJECT_CLASS(Group,"group");

int Group::addObject(const ObjectPtr& ob)
{
  assert(ob != nullptr);
  _objects.push_back(ob);
  return 0;
}

int Group::addLight(const LightPtr& lt)
{
  assert(lt != nullptr);
  if (!lt->trans()) { return -1; }
  _lights.push_back(lt);
  return 0;
}

int Group::init(Scene& s, const Transform* tr)
{
  for (auto& lt : _lights) {
    if (s.initLight(*lt, tr)) { return -1; }
  }

  for (auto& ob : _objects) {
    if (s.initObject(*ob, _shader, tr)) { return -1; }
  }

  ++s.group_count;
  return 0;
}

BBox Group::bound(const Matrix* t) const
{
  BBox b;
  for (auto& ob : _objects) {
    if (t == nullptr) {
      b.fit(ob->bound(nullptr));
    } else {
      assert(ob->trans());
      const Matrix t2 = ob->trans()->base * (*t);
      b.fit(ob->bound(&t2));
    }
  }
  return b;
}

int Group::intersect(const Ray& r, HitList& hl) const
{
  int hits = 0;
  for (auto& ob : _objects) { hits += ob->intersect(r, hl); }
  return hits;
}
