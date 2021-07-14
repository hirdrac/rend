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

class HitInfo : public SListNode<HitInfo>
{
 public:
  // basic hit information
  const Object* object;
  const Object* child; // sub-object hit for CSG
  Flt distance;
  Vec3 local_pt;
  int side;
  bool enter;

  HitInfo() = default;
  HitInfo(const Object* ob, Flt t, const Vec3& pt)
    : object(ob), child(nullptr), distance(t), local_pt(pt), side(0),
      enter(false) { }
};

class HitList
{
 public:
  HitList(SList<HitInfo>* cache = nullptr) : _freeCache(cache) { }
  ~HitList() { clear(); }

  // Member Functions
  void addHit(const Object* ob, Flt t, const Vec3& local_pt, int side,
              bool enter);
  void mergeList(HitList& list);
  void clear();

  [[nodiscard]] HitInfo* findFirstHit(const Ray& r);
  [[nodiscard]] HitInfo* extractFirst() { return _hitList.removeHead(); }
  [[nodiscard]] bool empty() const { return _hitList.empty(); }
  [[nodiscard]] int  count() const { return _hitList.count(); }

  void csgMerge(const Object* csg);
  void csgUnion(const Object* csg);
  void csgIntersection(const Object* csg, int objectCount);

  [[nodiscard]] SList<HitInfo>* freeCache() { return _freeCache; }

 private:
  SList<HitInfo> _hitList;
  SList<HitInfo>* _freeCache;

  void add(HitInfo* ht);
  void killNext(HitInfo* ht);
};
