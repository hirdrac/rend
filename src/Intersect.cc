//
// Intersect.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Intersect.hh"
#include "Ray.hh"


// **** HitList Class ****
void HitList::add(HitInfo* ht)
{
  HitInfo* prev = nullptr;
  HitInfo* h = _hitList.head();
  while (h && h->distance < ht->distance) { prev = h; h = h->next; }
  _hitList.addAfterNode(prev, ht);
}

void HitList::mergeList(HitList& list)
{
  HitInfo* prev = nullptr;
  HitInfo* h = _hitList.head();
  while (!list.empty()) {
    HitInfo* ht = list.removeHead();
    while (h && h->distance < ht->distance) { prev = h; h = h->next; }
    _hitList.addAfterNode(prev, ht);
    prev = ht;
    h = ht->next;
  }
}

void HitList::clear()
{
  _cache->store(_hitList);
}

HitInfo* HitList::removeFirstHit(const Ray& r)
{
  HitInfo* prev = nullptr;
  HitInfo* h = _hitList.head();
  while (h && (h->distance < r.min_length)) { prev = h; h = h->next; }

  return (h && (h->distance < r.max_length))
    ? _hitList.removeNext(prev) : nullptr;
}

void HitList::csgUnion(const Primitive* csg)
{
  HitInfo* h = _hitList.head();
  HitInfo* prev = nullptr;
  int insideCount = 0;

  while (h) {
    h->parent = csg; // claim hit as part of csg object
    bool remove;
    if (h->type == HIT_ENTER) {
      // entering solid object
      remove = ++insideCount > 1;
    } else if (h->type == HIT_EXIT) {
      // leaving object
      remove = --insideCount > 0;
    } else {
      // non-csg objects (plane/disc)
      remove = insideCount > 0;
    }

    HitInfo* next = h->next;
    if (remove) { killNext(prev); } else { prev = h; }
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
      h->parent = csg; // claim hit as part of csg object
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
    if (h->object == primary || h->parent == primary) {
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
      h->parent = csg; // claim hit as part of csg object
      prev = h;
      h = h->next;
    }
  }
}

HitInfo* HitList::newHit(Flt t)
{
  HitInfo* ht = _cache->fetch();
  ht->distance = t;
  ht->parent = nullptr;

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
  _cache->store(_hitList.removeNext(prev));
}
