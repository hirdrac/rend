//
// CSG.hh
// Copyright (C) 2021 Richard Bradley
//
// CSG object classes
//

#pragma once
#include "Object.hh"
#include "SList.hh"


// **** Class Definitions ****
class CSG : public Primitive
{
 public:
  // Constructor
  CSG();
  // Destructor
  ~CSG();

  // SceneItem Functions
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;

  // Object Functions
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override;
  Object* childList() const override { return _childList.head(); }

 protected:
  SList<Object> _childList;
  int _childCount = 0;
};


// **** CSG classes ****
class Merge final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc(int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Union final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc(int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Intersection final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc(int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Difference final : public CSG
{
 public:
  // SceneItem Functions
  std::string desc(int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};
