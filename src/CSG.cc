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

  BBox b = bound(nullptr);
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
BBox Merge::bound(const Matrix* t) const
{
  BBox b;
  for (auto& ob : _children) {
    if (t == nullptr) {
      b.fit(ob->bound(nullptr));
    } else {
      assert(ob->trans());
      Matrix t2 = ob->trans()->base * (*t);
      b.fit(ob->bound(&t2));
    }
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
BBox Union::bound(const Matrix* t) const
{
  BBox b;
  for (auto& ob : _children) {
    if (t == nullptr) {
      b.fit(ob->bound(nullptr));
    } else {
      assert(ob->trans());
      Matrix t2 = ob->trans()->base * (*t);
      b.fit(ob->bound(&t2));
    }
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
BBox Intersection::bound(const Matrix* t) const
{
  assert(!_children.empty());

  BBox b;
  if (t == nullptr) {
    b = _children[0]->bound(nullptr);
  } else {
    assert(_children[0]->trans());
    Matrix t2 = _children[0]->trans()->final() * (*t);
    b = _children[0]->bound(&t2);
  }

  for (std::size_t i = 1, size = _children.size(); i < size; ++i) {
    Object& ob = *_children[i];
    BBox b2;
    if (t == nullptr) {
      b2 = ob.bound(nullptr);
    } else {
      assert(ob.trans());
      Matrix t2 = ob.trans()->final() * (*t);
      b2 = ob.bound(&t2);
    }
    b.intersect(b2);
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
BBox Difference::bound(const Matrix* t) const
{
  assert(!_children.empty());
  Object& ob = *_children[0];
  if (t == nullptr) {
    return ob.bound(nullptr);
  } else {
    assert(ob.trans());
    Matrix t2 = ob.trans()->final() * (*t);
    return ob.bound(&t2);
  }
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
