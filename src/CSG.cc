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

Vec3 CSG::normal(const Ray& r, const HitInfo& h) const
{
  return h.child->normal(r, h);
}

Flt CSG::hitCost() const
{
  if (_cost >= 0.0) { return _cost; }
  Flt cost = CostTable.csg;
  for (auto& ob : _children) { cost += ob->hitCost(); }
  return cost;
}


// **** Merge Class ****
int Merge::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgMerge(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

BBox Merge::bound() const
{
  BBox b;
  for (auto& ob : _children) { b.fit(ob->bound()); }
  return b;
}


// **** Union Class ****
int Union::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgUnion(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

BBox Union::bound() const
{
  BBox b;
  for (auto& ob : _children) { b.fit(ob->bound()); }
  return b;
}


// **** Intersection Class ****
int Intersection::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgIntersection(this, int(_children.size()));

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

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


// **** Difference Class ****
int Difference::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl); }
  hl.csgDifference(this, _children.front().get());

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

BBox Difference::bound() const
{
  return _children.empty() ? BBox{} : _children.front()->bound();
}
