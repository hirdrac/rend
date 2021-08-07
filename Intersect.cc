//
// Intersect.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of intersect module
//

#include "Intersect.hh"
#include "Ray.hh"


// **** HitList Class ****
void HitList::addHit(const Object* ob, Flt t, const Vec3& pt, int s,
                     HitType type)
{
  HitInfo* ht = _freeCache ? _freeCache->fetch() : nullptr;
  if (!ht) { ht = new HitInfo; }

  ht->object    = ob;
  ht->child     = nullptr;
  ht->distance  = t;
  ht->local_pt  = pt;
  ht->side      = s;
  ht->type      = type;
  add(ht);
}

void HitList::mergeList(HitList& list)
{
  while (!list.empty()) { add(list.extractFirst()); }
}

void HitList::clear()
{
  if (_freeCache) {
    _freeCache->store(_hitList);
  } else {
    _hitList.purge();
  }
}

HitInfo* HitList::findFirstHit(const Ray& r)
{
  HitInfo* h = _hitList.head();
  while (h && (h->distance < r.min_length)) { h = h->next; }

  return (h && (h->distance < r.max_length)) ? h : nullptr;
}

void HitList::csgMerge(const Object* csg)
{
  for (HitInfo* h = _hitList.head(); h != nullptr; h = h->next) {
    // claim hit as part of csg object
    if (!h->child) { h->child = h->object; }
    h->object = csg;
  }
}

void HitList::csgUnion(const Object* csg)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  int insideCount = 0;

  while (h) {
    // claim hit as part of csg object
    const Object* ob = h->object;
    if (!h->child) { h->child = ob; }
    h->object = csg;

    if (h->type == HIT_ENTER) {
      // entering solid object
      ++insideCount;
    } else if (h->type == HIT_EXIT) {
      // leaving object
      --insideCount;
    }

    HitInfo* next = h->next;
    if (insideCount > 0) { killNext(prev); } else { prev = h; }
    h = next;
  }
}

void HitList::csgIntersection(const Object* csg, int objectCount)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  int count = 0; // inside count of objects

  while (h) {
    bool remove;
    if (h->type == HIT_ENTER) {
      // entering solid object
      remove = (++count < objectCount);
    } else if (h->type == HIT_EXIT) {
      // leaving solid object
      remove = (count-- < objectCount);
    } else {
      // hollow object hit
      // (allow intersection if it's inside all other objects)
      remove = (count < (objectCount-1));
    }

    if (remove) {
      h = h->next;
      killNext(prev);
    } else {
      // claim hit as part of csg object
      if (!h->child) { h->child = h->object; }
      h->object = csg;

      prev = h;
      h = h->next;
    }
  }
}

void HitList::csgDifference(const Object* csg, const Object* primary)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  int count = 0; // inside count of non-primary objects
  bool insidePrimary = false;

  while (h) {
    bool remove = true;
    if (h->object == primary) {
      insidePrimary = (h->type == HIT_ENTER);
      remove = (count > 0);
    } else if (h->type == HIT_ENTER) {
      ++count;
      h->type = HIT_EXIT;
      remove = !insidePrimary || (count != 1);
    } else if (h->type == HIT_EXIT) {
      --count;
      h->type = HIT_ENTER;
      remove = !insidePrimary || (count != 0);
    }

    if (remove) {
      h = h->next;
      killNext(prev);
    } else {
      // claim hit as part of csg object
      if (!h->child) { h->child = h->object; }
      h->object = csg;

      prev = h;
      h = h->next;
    }
  }
}

void HitList::add(HitInfo* ht)
{
  // sort hit into current hit list
  // (keep hit list sorted at all times)

  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  while (h && h->distance < ht->distance) { prev = h; h = h->next; }
  _hitList.addAfterNode(prev, ht);
}

void HitList::killNext(HitInfo* prev)
{
  HitInfo* h = _hitList.removeNext(prev);
  if (_freeCache) {
    _freeCache->store(h);
  } else {
    delete h;
  }
}
