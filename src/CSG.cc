//
// CSG.cc
// Copyright (C) 2021 Richard Bradley
//

#include "CSG.hh"
#include "Intersect.hh"
#include "Scene.hh"
#include "HitCostInfo.hh"
#include "Logger.hh"
#include "BBox.hh"
#include <algorithm>
#include <sstream>
#include <cassert>


// **** CSG Class ****
int CSG::addObject(const ObjectPtr& ob)
{
  assert(ob != nullptr);
  _children.push_back(ob);
  return 0;
}

int CSG::init(Scene& s)
{
  auto count = _children.size();
  if (count <= 1) {
    LOG_ERROR("Too few objects for ", desc());
    return -1;
  }

  for (auto& ob : _children) {
    if (InitObject(s, *ob, shader(), &_trans)) { return -1; }
  }

  BBox b = bound();
  if (b.empty()) {
    LOG_WARN("Empty bound for ", desc());
    return -1;
  }

  ++s.csg_count;
  return 0;
}

Flt CSG::hitCost() const
{
  if (_cost >= 0.0) { return _cost; }
  Flt cost = CostTable.csg;
  for (auto& ob : _children) { cost += ob->hitCost(); }
  return cost;
}

Vec3 CSG::normal(const Ray& r, const HitInfo& h) const
{
  return h.child->normal(r, h);
}


// **** Merge Class ****
BBox Merge::bound() const
{
  BBox b;
  for (auto& ob : _children) { b.fit(ob->bound()); }
  return b;
}

BBox Merge::localBound() const
{
  BBox b;
  for (auto& ob : _children) {
    InitObjectOnlyTransforms(*ob);
    b.fit(ob->bound());
  }
  return b;
}

int Merge::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgMerge(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}


// **** Union Class ****
BBox Union::bound() const
{
  BBox b;
  for (auto& ob : _children) { b.fit(ob->bound()); }
  return b;
}

BBox Union::localBound() const
{
  BBox b;
  for (auto& ob : _children) {
    InitObjectOnlyTransforms(*ob);
    b.fit(ob->bound());
  }
  return b;
}

int Union::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgUnion(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}


// **** Intersection Class ****
BBox Intersection::bound() const
{
  if (_children.empty()) { return {}; }

  BBox b = _children[0]->bound();
  for (std::size_t i = 1, size = _children.size(); i < size; ++i) {
    BBox b2 = _children[i]->bound();
    b.intersect(b2);
  }

  return b;
}

BBox Intersection::localBound() const
{
  if (_children.empty()) { return {}; }

  InitObjectOnlyTransforms(*_children[0]);
  BBox b = _children[0]->bound();
  for (std::size_t i = 1, size = _children.size(); i < size; ++i) {
    Object& child = *_children[i];
    InitObjectOnlyTransforms(child);
    b.intersect(child.bound());
  }
  return b;
}

int Intersection::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgIntersection(this, int(_children.size()));

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}


// **** Difference Class ****
BBox Difference::bound() const
{
  return _children.empty() ? BBox{} : _children[0]->bound();
}

BBox Difference::localBound() const
{
  if (_children.empty()) { return {}; }

  Object& ob = *_children[0];
  InitObjectOnlyTransforms(ob);
  return ob.bound();
}

int Difference::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgDifference(this, _children[0].get());

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}
