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

  objects.push_back(ob);
  return 0;
}

int CSG::init(Scene& s)
{
  if (objects.size() <= 1) {
    println_err("Too few objects");
    return -1;
  }

  for (auto& ob : objects) {
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

Flt CSG::hitCost(const HitCostInfo& hc) const
{
  if (_cost >= 0.0) { return _cost; }
  Flt cost = hc.csg;
  for (auto& ob : objects) {
    auto pPtr = dynamic_cast<const Primitive*>(ob.get());
    assert(pPtr != nullptr);
    cost += pPtr->hitCost(hc);
  }
  return cost;
}


// **** Union Class ****
BBox Union::bound(const Matrix* t) const
{
  BBox b;
  for (auto& ob : objects) {
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
  HitList hl2(hl.cache(), hl.stats(), true);
  for (auto& ob : objects) { ob->intersect(r, hl2); }
  hl2.csgUnion(this);

  const int hits = hl2.count();
  hl.mergeList(hl2);
  return hits;
}


// **** Intersection Class ****
BBox Intersection::bound(const Matrix* t) const
{
  assert(!objects.empty());

  BBox b;
  if (t == nullptr) {
    b = objects[0]->bound(nullptr);
  } else {
    assert(objects[0]->trans());
    const Matrix t2 = objects[0]->trans()->base * (*t);
    b = objects[0]->bound(&t2);
  }

  for (std::size_t i = 1, size = objects.size(); i < size; ++i) {
    Object& ob = *objects[i];
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
  HitList hl2(hl.cache(), hl.stats(), true);
  for (auto& ob : objects) { ob->intersect(r, hl2); }
  hl2.csgIntersection(this, int(objects.size()));

  const int hits = hl2.count();
  hl.mergeList(hl2);
  return hits;
}


// **** Difference Class ****
BBox Difference::bound(const Matrix* t) const
{
  assert(!objects.empty());
  Object& ob = *objects[0];
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
  HitList hl2(hl.cache(), hl.stats(), true);
  for (auto& ob : objects) { ob->intersect(r, hl2); }
  hl2.csgDifference(this, objects[0].get());

  const int hits = hl2.count();
  hl.mergeList(hl2);
  return hits;
}
