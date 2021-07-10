//
// Object.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of object classes
//

#include "Object.hh"
#include "Shader.hh"
#include "Stats.hh"
#include "Print.hh"
#include <algorithm>


// **** BBox class ****
BBox::BBox(const BBox& b1, const BBox& b2)
{
  pmin.x = std::min(b1.pmin.x, b2.pmin.x);
  pmin.y = std::min(b1.pmin.y, b2.pmin.y);
  pmin.z = std::min(b1.pmin.z, b2.pmin.z);

  pmax.x = std::max(b1.pmax.x, b2.pmax.x);
  pmax.y = std::max(b1.pmax.y, b2.pmax.y);
  pmax.z = std::max(b1.pmax.z, b2.pmax.z);
}

void BBox::reset()
{
  pmin.set( VERY_LARGE,  VERY_LARGE,  VERY_LARGE);
  pmax.set(-VERY_LARGE, -VERY_LARGE, -VERY_LARGE);
}

Flt BBox::weight() const
{
  Flt x = Abs(pmax.x - pmin.x);
  Flt y = Abs(pmax.y - pmin.y);
  Flt z = Abs(pmax.z - pmin.z);
  return (x * (y + z)) + (y * z);
}

void BBox::fit(const Vec3& p)
{
  pmin.x = std::min(pmin.x, p.x);
  pmin.y = std::min(pmin.y, p.y);
  pmin.z = std::min(pmin.z, p.z);

  pmax.x = std::max(pmax.x, p.x);
  pmax.y = std::max(pmax.y, p.y);
  pmax.z = std::max(pmax.z, p.z);
}

void BBox::fit(const BBox& box)
{
  pmin.x = std::min(pmin.x, box.pmin.x);
  pmin.y = std::min(pmin.y, box.pmin.y);
  pmin.z = std::min(pmin.z, box.pmin.z);

  pmax.x = std::max(pmax.x, box.pmax.x);
  pmax.y = std::max(pmax.y, box.pmax.y);
  pmax.z = std::max(pmax.z, box.pmax.z);
}

void BBox::fit(const Transform& t, const Vec3* pt_list, int pt_count)
{
  for (int i = 0; i < pt_count; ++i) {
    fit(MultPoint(pt_list[i], t.global));
  }
}


// **** Object Class ****
const ShaderPtr Object::_nullShader;
const std::vector<ObjectPtr> Object::_emptyList;

Object::Object()
{
  ++Inventory.objects;
}

Object::~Object()
{
  --Inventory.objects;
}


// **** Primitive Class ****
// SceneItem Functions
int Primitive::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || _shader) { return -1; }

  _shader = sh;
  return 0;
}

int Primitive::init(Scene& s)
{
  return _trans.init();
}

// Object Functions
int Primitive::bound(BBox& b) const
{
  static constexpr Vec3 pt[8] = {
    { 1, 1, 1}, {-1, 1, 1}, { 1,-1, 1}, { 1, 1,-1},
    {-1,-1, 1}, { 1,-1,-1}, {-1, 1,-1}, {-1,-1,-1}};
  b.fit(_trans, pt, 8);
  return 0;
}


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh, const Transform* t)
{
  Transform* trans = ob.trans();
  if (!trans) {
    println("No Trans ERROR: ", ob.desc());
    return -1;
  }

  // Transform by parent trans
  trans->global = trans->local;
  if (t) {
    trans->global *= t->global;
  }

  // Assign default shader if none is set
  if (!ob.shader()) { ob.setShader(sh); }

  // Init primitive
  int error = ob.init(s);
  if (error) {
    println("INIT ERROR: ", ob.desc());
    return error;
  }

  return 0;
}
