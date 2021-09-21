//
// Intersect.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Intersect.hh"
#include "Ray.hh"


// **** HitList Class ****
void HitList::mergeList(HitList& list)
{
  HitInfo* prev = nullptr;
  HitInfo* h = _hitList.head();
  while (!list.empty()) {
    HitInfo* ht = list.extractFirst();
    while (h && h->distance < ht->distance) { prev = h; h = h->next; }
    _hitList.addAfterNode(prev, ht);
    prev = ht;
    h = ht->next;
  }
}

void HitList::clear()
{
  _freeCache->store(_hitList);
}

const HitInfo* HitList::findFirstHit(const Ray& r) const
{
  const HitInfo* h = _hitList.head();
  while (h && (h->distance < r.min_length)) { h = h->next; }

  return (h && (h->distance < r.max_length)) ? h : nullptr;
}

void HitList::csgUnion(const Primitive* csg)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  int insideCount = 0;

  while (h) {
    // claim hit as part of csg object
    const Primitive* ob = h->object;
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

void HitList::csgIntersection(const Primitive* csg, int objectCount)
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

void HitList::csgDifference(const Primitive* csg, const void* primary)
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

HitInfo* HitList::newHit(Flt t)
{
  HitInfo* ht = _freeCache->fetch();
  ht->distance = t;
  ht->child = nullptr;

  // sort hit into current hit list
  // (keep hit list sorted at all times)
  HitInfo* prev = nullptr;
  HitInfo* h = _hitList.head();
  while (h && h->distance < t) { prev = h; h = h->next; }
  _hitList.addAfterNode(prev, ht);
  return ht;
}

void HitList::killNext(HitInfo* prev)
{
  _freeCache->store(_hitList.removeNext(prev));
}
