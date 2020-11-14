//
// CSG.hh - revision 22 (2019/1/1)
// Copyright(C) 2019 by Richard Bradley
//
// CSG object classes
//

#ifndef CSG_hh
#define CSG_hh

#include "Object.hh"
#include "SList.hh"
#include <iostream>


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
  Object* childList() const override { return child_list.head(); }

 protected:
  SList<Object> child_list;
  int o_count = 0;
};


// **** CSG classes ****
class Merge : public CSG
{
 public:
  // SceneItem Functions
  void print(std::ostream& stream, int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Union : public CSG
{
 public:
  // SceneItem Functions
  void print(std::ostream& stream, int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Intersection : public CSG
{
 public:
  // SceneItem Functions
  void print(std::ostream& stream, int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

class Difference : public CSG
{
 public:
  // SceneItem Functions
  void print(std::ostream& stream, int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int bound(BBox& b) const override;
};

#endif
