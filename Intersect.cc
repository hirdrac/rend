//
// Intersect.cc - revision 27 (2019/1/6)
// Copyright(C) 2019 by Richard Bradley
//
// Implementation of intersect module
//

#include "Intersect.hh"
#include "Object.hh"
#include "Ray.hh"
#include <set>


/**** HitList Class ****/
// Member Functions
int HitList::addHit(const Object* ob, Flt t, const Vec3& pt, int s)
{
  HitInfo* ht = _freeCache ? _freeCache->removeHead() : nullptr;
  if (!ht) { ht = new HitInfo; }

  ht->object    = ob;
  ht->child     = nullptr;
  ht->distance  = t;
  ht->local_pt  = pt;
  ht->global_pt = {0,0,0};
  ht->side      = s;

  return add(ht);
}

int HitList::mergeList(HitList& list)
{
  while (!list.empty()) { add(list.extractFirst()); }
  return 0;
}

void HitList::clear()
{
  if (_freeCache) {
    _freeCache->addToTail(_hitList);
  } else {
    _hitList.purge();
  }
}

HitInfo* HitList::findFirstHit(const Ray& r) const
{
  HitInfo* h = _hitList.head();
  while (h && (h->distance < VERY_SMALL)) { h = h->next(); }

  return (h && (h->distance < r.max_length)) ? h : nullptr;
}

int HitList::csgMerge(const Object* csg)
{
  for (HitInfo* h = _hitList.head(); h != nullptr; h = h->next()) {
    // claim hit as part of csg object
    if (!h->child) { h->child = h->object; }
    h->object = csg;
  }

  return 0;
}

int HitList::csgUnion(const Object* csg)
{
  HitInfo* h = _hitList.head();
  std::set<const void*> insideSet;

  while (h) {
    HitInfo* next = h->next();

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

    if (!insideSet.empty()) { kill(h); }
    h = next;
  }

  return 0;
}

int HitList::csgIntersection(const Object* csg, int objectCount)
{
  HitInfo* h = _hitList.head();
  std::set<const void*> insideSet;
  int count = 0;

  while (h) {
    HitInfo* next = h->next();

    // claim hit as part of csg object
    const Object* ob = h->object;
    if (!h->child) { h->child = ob; }
    h->object = csg;

    if (!ob->isSolid()) {
      // hollow object
      kill(h);

    } else if (insideSet.find(ob) != insideSet.end()) {
      // leaving solid object
      if (count < objectCount) { kill(h); }
      insideSet.erase(ob);
      --count;

    } else {
      // entering solid object
      insideSet.insert(ob);
      if (++count < objectCount) { kill(h); }
    }

    h = next;
  }

  return 0;
}

int HitList::add(HitInfo* ht)
{
  // sort hit into current hit list
  // (keep hit list sorted at all times)

  HitInfo* h = _hitList.tail();
  while (h && h->distance > ht->distance) { h = h->fore(); }

  if (h) {
    _hitList.addAfterNode(h, ht);
  } else {
    _hitList.addToHead(ht);
  }

  return 0;
}

void HitList::kill(HitInfo* h)
{
  _hitList.remove(h);
  if (_freeCache) {
    _freeCache->addToTail(h);
  } else {
    delete h;
  }
}

std::ostream& operator<<(std::ostream& out, const HitInfo& h)
{
  return out << "<Hit " << h.distance << ' ' << *(h.object) << '>';
}
