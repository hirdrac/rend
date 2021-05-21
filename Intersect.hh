//
// Intersect.hh
// Copyright (C) 2021 Richard Bradley
//
// Intersection class and calculations
//

#pragma once
#include "Types.hh"
#include "DList.hh"
#include <iosfwd>


// **** Types ****
class Object;
class Ray;

class HitInfo : public DListNode<HitInfo>
{
 public:
  // basic hit information
  const Object* object;
  const Object* child;
  Flt distance;
  Vec3 local_pt;
  Vec3 global_pt; // calculated for shaders
  int side;

  // Constructors
  HitInfo() = default;
  HitInfo(const Object* ob, Flt t, const Vec3& pt)
    : object(ob), child(nullptr), distance(t),
      local_pt(pt), global_pt(), side(0) { }
};

class HitList
{
 public:
  // Constructor
  HitList(DList<HitInfo>* cache = nullptr) : _freeCache(cache) { }
  // Destructor
  ~HitList() { clear(); }

  // Member Functions
  int addHit(const Object* ob, Flt t, const Vec3& local_pt, int side = 0);
  int mergeList(HitList& list);
  void clear();

  [[nodiscard]] HitInfo* findFirstHit(const Ray& r) const;
  [[nodiscard]] HitInfo* extractFirst() { return _hitList.removeHead(); }
  [[nodiscard]] bool empty() const { return _hitList.empty(); }
  [[nodiscard]] int  count() const { return _hitList.count(); }

  int csgMerge(const Object* csg);
  int csgUnion(const Object* csg);
  int csgIntersection(const Object* csg, int objectCount);

  [[nodiscard]] DList<HitInfo>* freeCache() { return _freeCache; }

 private:
  DList<HitInfo> _hitList;
  DList<HitInfo>* _freeCache;

  int add(HitInfo* ht);
  void kill(HitInfo* ht);
};


// **** Functions ****
std::ostream& operator<<(std::ostream& os, const HitInfo& h);
