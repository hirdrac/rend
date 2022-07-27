//
// HitInfo.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "SList.hh"
#include "Types.hh"


// **** Types ****
class Primitive;

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
