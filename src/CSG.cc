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


// **** CSG Class ****
int CSG::addObject(const ObjectPtr& ob)
{
  if (!ob) { return -1; }

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

  BBox b;
  bound(b);
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

int Merge::bound(BBox& b) const
{
  for (auto& ob : _children) { ob->bound(b); }
  return 0;
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

int Union::bound(BBox& b) const
{
  for (auto& ob : _children) { ob->bound(b); }
  return 0;
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

int Intersection::bound(BBox& b) const
{
  if (_children.empty()) { return -1; }

  _children[0]->bound(b);
  for (std::size_t i = 1, size = _children.size(); i < size; ++i) {
    BBox b2;
    _children[i]->bound(b2);
    b.intersect(b2);
  }

  return 0;
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

int Difference::bound(BBox& b) const
{
  return _children.empty() ? -1 : _children.front()->bound(b);
}
