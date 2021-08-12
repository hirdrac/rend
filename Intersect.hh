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
class Object;
class Ray;

struct EvaluatedHit {
  Vec3 global_pt;
  Vec3 normal;
  Vec3 map;
};

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
  const Object* object;
  const Object* child; // sub-object hit for CSG
  Flt distance;
  Vec3 local_pt;
  int side;
  HitType type;

  HitInfo() = default;
  HitInfo(const Object* ob, Flt t, const Vec3& pt)
    : object(ob), child(nullptr), distance(t), local_pt(pt), side(0),
      type(HIT_NORMAL) { }
};

class HitCache
{
 public:
  [[nodiscard]] HitInfo* fetch() { return _cache.removeHead(); }
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
  void addHit(const Object* ob, Flt t, const Vec3& local_pt, int side,
              HitType type);
  void mergeList(HitList& list);
  void clear();

  [[nodiscard]] HitInfo* findFirstHit(const Ray& r);
  [[nodiscard]] HitInfo* extractFirst() { return _hitList.removeHead(); }
  [[nodiscard]] bool empty() const { return _hitList.empty(); }
  [[nodiscard]] int  count() const { return _hitList.count(); }
  [[nodiscard]] bool csg() const { return _csg; }

  void csgMerge(const Object* csg);
  void csgUnion(const Object* csg);
  void csgIntersection(const Object* csg, int objectCount);
  void csgDifference(const Object* csg, const Object* primary);

  [[nodiscard]] HitCache* freeCache() { return _freeCache; }

 private:
  SList<HitInfo> _hitList;
  HitCache* _freeCache;
  bool _csg;

  void killNext(HitInfo* ht);
};
