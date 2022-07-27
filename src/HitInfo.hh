//
// HitInfo.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "ListUtility.hh"
#include "Types.hh"
#include <utility>


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
  ~HitCache() { KillNodes(_head); }

  [[nodiscard]] HitInfo* fetch() {
    if (_head) {
      HitInfo* ht = _head;
      _head = std::exchange(ht->next, nullptr);
      return ht;
    } else {
      return new HitInfo;
    }
  }

  void store(HitInfo* h) {
    h->next = _head; _head = h; }
  void storeList(HitInfo* head, HitInfo* tail) {
    tail->next = _head; _head = head; }

 private:
  HitInfo* _head = nullptr;
};
