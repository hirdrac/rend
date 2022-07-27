//
// Intersect.hh
// Copyright (C) 2022 Richard Bradley
//
// Intersection class and calculations
//

#pragma once
#include "HitInfo.hh"
#include "SList.hh"
#include "Types.hh"


// **** Types ****
class Primitive;
class Ray;
class StatInfo;

class HitList
{
 public:
  HitList(HitCache& cache, StatInfo& stats, bool csg)
    : _cache{&cache}, _stats{&stats}, _csg{csg} { }
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

  void add(HitInfo* ht);
  void mergeList(HitList& list);
  void clear();

  [[nodiscard]] const HitInfo* firstHit() const { return _hitList.head(); }
  [[nodiscard]] HitInfo* removeFirstHit(const Ray& r);

  [[nodiscard]] HitInfo* removeHead() { return _hitList.removeHead(); }
  [[nodiscard]] bool empty() const { return _hitList.empty(); }
  [[nodiscard]] int  count() const { return _hitList.count(); }
  [[nodiscard]] bool csg() const { return _csg; }

  void csgUnion(const Primitive* csg);
  void csgIntersection(const Primitive* csg, int objectCount);
  void csgDifference(const Primitive* csg, const void* primary);

  [[nodiscard]] HitCache& cache() { return *_cache; }
  [[nodiscard]] StatInfo& stats() { return *_stats; }

 private:
  SList<HitInfo> _hitList;
  HitCache* _cache;
  StatInfo* _stats;
  bool _csg;

  [[nodiscard]] HitInfo* newHit(Flt t);
  void killNext(HitInfo* ht);
};
