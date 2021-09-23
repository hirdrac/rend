//
// CSG.cc
// Copyright (C) 2021 Richard Bradley
//

#include "CSG.hh"
#include "Intersect.hh"
#include "Scene.hh"
#include "HitCostInfo.hh"
#include "BBox.hh"
#include "Print.hh"
#include <cassert>


// **** CSG Class ****
int CSG::addObject(const ObjectPtr& ob)
{
  assert(ob != nullptr);
  if (!dynamic_cast<const Primitive*>(ob.get())) {
    println("Can't add non-primitive object to CSG");
    return -1;
  }

  _children.push_back(ob);
  return 0;
}

int CSG::init(Scene& s)
{
  if (_children.size() <= 1) {
    println_err("Too few objects");
    return -1;
  }

  for (auto& ob : _children) {
    if (InitObject(s, *ob, shader(), &_trans)) { return -1; }
  }

  const BBox b = bound(nullptr);
  if (b.empty()) {
    println_err("Empty bound");
    return -1;
  }

  ++s.csg_count;
  return 0;
}

Flt CSG::hitCost() const
{
  if (_cost >= 0.0) { return _cost; }
  Flt cost = CostTable.csg;
  for (auto& ob : _children) {
    auto pPtr = dynamic_cast<const Primitive*>(ob.get());
    assert(pPtr != nullptr);
    cost += pPtr->hitCost();
  }
  return cost;
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
      const Matrix t2 = ob->trans()->base * (*t);
      b.fit(ob->bound(&t2));
    }
  }
  return b;
}

int Union::intersect(const Ray& r, HitList& hl) const
{
  HitList hl2(hl.cache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl2); }
  hl2.csgUnion(this);

  const int hits = hl2.count();
  hl.mergeList(hl2);
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
    const Matrix t2 = _children[0]->trans()->base * (*t);
    b = _children[0]->bound(&t2);
  }

  for (std::size_t i = 1, size = _children.size(); i < size; ++i) {
    Object& ob = *_children[i];
    BBox b2;
    if (t == nullptr) {
      b2 = ob.bound(nullptr);
    } else {
      assert(ob.trans());
      const Matrix t2 = ob.trans()->base * (*t);
      b2 = ob.bound(&t2);
    }
    b.intersect(b2);
  }

  return b;
}

int Intersection::intersect(const Ray& r, HitList& hl) const
{
  HitList hl2(hl.cache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl2); }
  hl2.csgIntersection(this, int(_children.size()));

  const int hits = hl2.count();
  hl.mergeList(hl2);
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
    const Matrix t2 = ob.trans()->base * (*t);
    return ob.bound(&t2);
  }
}

int Difference::intersect(const Ray& r, HitList& hl) const
{
  HitList hl2(hl.cache(), true);
  for (auto& ob : _children) { ob->intersect(r, hl2); }
  hl2.csgDifference(this, _children[0].get());

  const int hits = hl2.count();
  hl.mergeList(hl2);
  return hits;
}
