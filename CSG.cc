//
// CSG.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of CSG classes
//

#include "CSG.hh"
#include "Intersect.hh"
#include "Stats.hh"
#include "HitCostInfo.hh"
#include "Logger.hh"
#include <algorithm>
#include <sstream>


/**** CSG Class ****/
// Constructor
CSG::CSG()
{
  ++Inventory.csgs;
}

// Destructor
CSG::~CSG()
{
  --Inventory.csgs;
}

// Member Functions
int CSG::add(SceneItem* i, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(i);
  if (ob) {
    child_list.addToTail(ob);
    return 0;
  } else {
    return Primitive::add(i, flag);
  }
}

int CSG::init(Scene& s)
{
  o_count = child_list.count();
  if (o_count <= 1) {
    LOG_ERROR("Too few objects for CSG (", o_count, ")");
    return -1;
  }

  return InitObjectList(s, child_list.head(), shader(), &_trans);
}

int CSG::evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const
{
  return h.child->evalHit(h, normal, map);
}

Flt CSG::hitCost() const
{
  Flt cost = CostTable.csg;
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    cost += ob->hitCost();
  }

  return cost;
}


/**** Merge Class ****/
int Merge::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hl);
  }
  hl.csgMerge(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

int Merge::bound(BBox& b) const
{
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->bound(b);
  }

  return 0;
}

std::string Merge::desc(int indent) const
{
  std::ostringstream os;
  os << "<Merge>";
  if (!child_list.empty()) {
    os << '\n';
    PrintList(os, child_list.head(), indent + 2);
    os << '\n';
  }
  return os.str();
}


/**** Union Class ****/
int Union::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hl);
  }
  hl.csgUnion(this);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

int Union::bound(BBox& b) const
{
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->bound(b);
  }

  return 0;
}

std::string Union::desc(int indent) const
{
  std::ostringstream os;
  os << "<Union>";
  if (!child_list.empty()) {
    os << '\n';
    PrintList(os, child_list.head(), indent + 2);
    os << '\n';
  }
  return os.str();
}


/**** Intersection Class ****/
int Intersection::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hl);
  }
  hl.csgIntersection(this, o_count);

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

int Intersection::bound(BBox& b) const
{
  Flt best_weight = VERY_LARGE;
  BBox box;

  // find smallest bounding box
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    box.reset();
    ob->bound(box);
    Flt weight = box.weight();
    if (weight < best_weight) {
      best_weight = weight;
      b = box;
    }
  }

  return 0;
}

std::string Intersection::desc(int indent) const
{
  std::ostringstream os;
  os << "<Intersection>";
  if (!child_list.empty()) {
    os << '\n';
    PrintList(os, child_list.head(), indent + 2);
    os << '\n';
  }
  return os.str();
}


/**** Difference Class ****/
int Difference::intersect(const Ray& r, HitList& hit_list) const
{
  HitList hl(hit_list.freeCache());
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hl);
  }
  //hl.csgDifference(this, child_list.head(), o_count - 1);
  // FIXME - finish

  int hits = hl.count();
  hit_list.mergeList(hl);
  return hits;
}

int Difference::bound(BBox& b) const
{
  if (child_list.empty()) {
    return -1;
  }

  return child_list.head()->bound(b);
}

std::string Difference::desc(int indent) const
{
  std::ostringstream os;
  os << "<Difference>";
  if (!child_list.empty()) {
    os << '\n';
    PrintList(os, child_list.head(), indent + 2);
    os << '\n';
  }
  return os.str();
}
