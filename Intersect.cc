//
// Intersect.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of intersect module
//

#include "Intersect.hh"
#include "Object.hh"
#include "Ray.hh"
#include <set>


// **** HitList Class ****
// Member Functions
void HitList::addHit(const Object* ob, Flt t, const Vec3& pt, int s)
{
  HitInfo* ht = _freeCache ? _freeCache->removeHead() : nullptr;
  if (!ht) { ht = new HitInfo; }

  ht->object    = ob;
  ht->child     = nullptr;
  ht->distance  = t;
  ht->local_pt  = pt;
  ht->global_pt = {0,0,0};
  ht->side      = s;
  add(ht);
}

void HitList::mergeList(HitList& list)
{
  while (!list.empty()) { add(list.extractFirst()); }
}

void HitList::clear()
{
  if (_freeCache) {
    _freeCache->addToTail(_hitList);
  } else {
    _hitList.purge();
  }
}

HitInfo* HitList::findFirstHit(const Ray& r)
{
  HitInfo* h = _hitList.head();
  while (h && (h->distance < VERY_SMALL)) { h = h->next(); }

  return (h && (h->distance < r.max_length)) ? h : nullptr;
}

void HitList::csgMerge(const Object* csg)
{
  for (HitInfo* h = _hitList.head(); h != nullptr; h = h->next()) {
    // claim hit as part of csg object
    if (!h->child) { h->child = h->object; }
    h->object = csg;
  }
}

void HitList::csgUnion(const Object* csg)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  std::set<const void*> insideSet;

  while (h) {
    // claim hit as part of csg object
    const Object* ob = h->object;
    if (!h->child) { h->child = ob; }
    h->object = csg;

    if (ob->isSolid()) {
      // solid object hit
      if (insideSet.find(ob) != insideSet.end()) {
	// leaving object
	insideSet.erase(ob);
      } else {
	// entering object
	insideSet.insert(ob);
      }
    }

    HitInfo* next = h->next();
    if (!insideSet.empty()) { killNext(prev); } else { prev = h; }
    h = next;
  }
}

void HitList::csgIntersection(const Object* csg, int objectCount)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  std::set<const void*> insideSet;
  int count = 0;

  while (h) {
    // claim hit as part of csg object
    const Object* ob = h->object;
    if (!h->child) { h->child = ob; }
    h->object = csg;

    HitInfo* next = h->next();
    if (!ob->isSolid()) {
      // hollow object
      killNext(prev);
    } else if (insideSet.find(ob) != insideSet.end()) {
      // leaving solid object
      if (count < objectCount) { killNext(prev); } else { prev = h; }
      insideSet.erase(ob);
      --count;
    } else {
      // entering solid object
      insideSet.insert(ob);
      if (++count < objectCount) { killNext(prev); } else { prev = h; }
    }

    h = next;
  }
}

void HitList::add(HitInfo* ht)
{
  // sort hit into current hit list
  // (keep hit list sorted at all times)

  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  while (h && h->distance < ht->distance) { prev = h; h = h->next(); }
  _hitList.addAfterNode(prev, ht);
}

void HitList::killNext(HitInfo* prev)
{
  HitInfo* h = _hitList.removeNext(prev);
  if (_freeCache) {
    _freeCache->addToTail(h);
  } else {
    delete h;
  }
}
