//
// Bound.hh - revision 20 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//
// Defintion of Bound object class and
// bounding box heiarchy function
//

#ifndef Bound_hh
#define Bound_hh

#include "Object.hh"
#include "HitCostInfo.hh"
#include <vector>


// **** Types ****
class Bound : public Object
{
 public:
  SList<Object> child_list;
  std::vector<const Object*> object_list;
  BBox box;
  bool always_hit = false;

  // Constructor
  Bound();
  // Destructor
  ~Bound();

  // SceneItem Functions
  void print(std::ostream& stream, int) const override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  Flt hitCost() const override { return CostTable.bound; }
};


// **** Functions ****
Bound* MakeBoundList(const Object* list);

#endif
