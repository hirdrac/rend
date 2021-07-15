//
// CSG.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of CSG classes
//

#include "CSG.hh"
#include "Intersect.hh"
#include "Stats.hh"
#include "HitCostInfo.hh"
#include "Logger.hh"
#include <algorithm>
#include <sstream>


// **** CSG Class ****
CSG::CSG()
{
  ++Inventory.csgs;
}

CSG::~CSG()
{
  --Inventory.csgs;
}

// Member Functions
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

  return 0;
}

int CSG::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  return h.child->evalHit(h, normal, map);
}

Flt CSG::hitCost() const
{
  Flt cost = CostTable.csg;
  for (auto& ob : _children) { cost += ob->hitCost(); }
  return cost;
}


// **** Merge Class ****
int Merge::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
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
  HitList hl(hit_list.freeCache());
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
  HitList hl(hit_list.freeCache());
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

    if (b2.pmin.x > b.pmin.x) { b.pmin.x = b2.pmin.x; }
    if (b2.pmin.y > b.pmin.y) { b.pmin.y = b2.pmin.y; }
    if (b2.pmin.z > b.pmin.z) { b.pmin.z = b2.pmin.z; }

    if (b2.pmax.x < b.pmax.x) { b.pmax.x = b2.pmax.x; }
    if (b2.pmax.y < b.pmax.y) { b.pmax.y = b2.pmax.y; }
    if (b2.pmax.z < b.pmax.z) { b.pmax.z = b2.pmax.z; }
  }

  return 0;
}


// **** Difference Class ****
int Difference::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
  for (auto& ob : _children) { ob->intersect(r, hl); }
  //hl.csgDifference(this, _childList.head(), _childCount - 1);
  // FIXME - finish

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

int Difference::bound(BBox& b) const
{
  return _children.empty() ? -1 : _children.front()->bound(b);
}
