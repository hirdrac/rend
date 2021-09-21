//
// Intersect.hh
// Copyright (C) 2021 Richard Bradley
//
// Intersection class and calculations
//

#pragma once
#include "Types.hh"
#include "SList.hh"


// **** Types ****
class Primitive;
class Ray;

enum HitType {
  HIT_NORMAL, // non-CSG surface hit
  HIT_ENTER,  // enter CSG solid
  HIT_EXIT,   // exit CSG solid
};

class HitInfo
{
 public:
  // basic hit information
  HitInfo* next = nullptr;
  const Primitive* object;
  const Primitive* parent; // CSG object
  Flt distance;
  Vec3 local_pt;
  int side;
  HitType type;

  HitInfo() = default;
  HitInfo(const Primitive* ob, Flt t, const Vec3& pt)
    : object(ob), parent(nullptr), distance(t), local_pt(pt), side(0),
      type(HIT_NORMAL) { }
};

class HitCache
{
 public:
  [[nodiscard]] HitInfo* fetch() {
    HitInfo* ht = _cache.removeHead();
    return ht ? ht : new HitInfo;
  }

  void store(HitInfo* h) { _cache.addToHead(h); }
  void store(SList<HitInfo>& c) { _cache.addToHead(c); }

 private:
  SList<HitInfo> _cache;
};

class HitList
{
 public:
  HitList(HitCache* cache, bool csg) : _freeCache(cache), _csg(csg) { }
  ~HitList() { clear(); }

  // Member Functions
  void addHit(const Primitive* ob, Flt t, const Vec3& local_pt, int side,
              HitType type) {
    HitInfo* h = newHit(t);
    h->object   = ob;
    h->local_pt = local_pt;
    h->side     = side;
    h->type     = type;
  }

  void mergeList(HitList& list);
  void clear();

  [[nodiscard]] const HitInfo* findFirstHit(const Ray& r) const;
  [[nodiscard]] HitInfo* extractFirst() { return _hitList.removeHead(); }
  [[nodiscard]] bool empty() const { return _hitList.empty(); }
  [[nodiscard]] int  count() const { return _hitList.count(); }
  [[nodiscard]] bool csg() const { return _csg; }

  void csgUnion(const Primitive* csg);
  void csgIntersection(const Primitive* csg, int objectCount);
  void csgDifference(const Primitive* csg, const void* primary);

  [[nodiscard]] HitCache* freeCache() { return _freeCache; }

 private:
  SList<HitInfo> _hitList;
  HitCache* _freeCache;
  bool _csg;

  [[nodiscard]] HitInfo* newHit(Flt t);
  void killNext(HitInfo* ht);
};
