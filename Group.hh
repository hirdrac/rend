//
// Group.hh - revision 1 (2018/12/31)
// Copyright(C) 2018 by Richard Bradley
//

#ifndef Group_hh
#define Group_hh

#include "Object.hh"
#include "SList.hh"


// **** Types ****
// Group class
//  class for grouping together objects for transformations, etc
class Group final : public Primitive
{
 public:
  // Constructor
  Group();
  // Destructor
  ~Group();

  // SceneItem Functions
  void print(std::ostream& stream, int indent = 0) const override;
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;

  // Object Functions
  int bound(BBox& b) const override { return -1; }
  int intersect(const Ray& r, HitList& hit_list) const override;
  Object* childList() const override { return child_list.head(); }

 protected:
  SList<Object> child_list;
};

#endif
