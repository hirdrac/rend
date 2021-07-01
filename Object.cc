//
// Object.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of object classes
//

#include "Object.hh"
#include "Scene.hh"
#include "Shader.hh"
#include "Intersect.hh"
#include "Stats.hh"
#include "Print.hh"
#include <algorithm>
#include <cstring>


// **** BBox class ****
// Member Functions
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


// **** Object Class ****
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
int Primitive::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh || _shader) {
    return -1;
  }

  _shader = sh;
  return 0;
}

int Primitive::init(Scene& s)
{
  _trans.init();
  return Object::init(s);
}

// Object Functions
int Primitive::bound(BBox& b) const
{
  static constexpr Vec3 pt[8] = {
    { 1, 1, 1}, {-1, 1, 1}, { 1,-1, 1}, { 1, 1,-1},
    {-1,-1, 1}, { 1,-1,-1}, {-1, 1,-1}, {-1,-1,-1}};
  return MakeBound(b, _trans, pt, 8);
}


// **** Functions ****
int InitObjectList(Scene& s, Object* list, Shader* sh, const Transform* t)
{
  for (; list != nullptr; list = list->next()) {
    Transform* trans = list->trans();
    if (!trans) {
      println("No Trans ERROR: ", list->desc());
      return -1;
    }

    // Transform by parent trans
    trans->global = trans->local;
    if (t) {
      trans->global *= t->global;
    }

    // Assign default shader if none is set
    if (!list->shader()) {
      list->setShader(sh);
    }

    // Init primitive
    int error = list->init(s);
    if (error) {
      println("INIT ERROR: ", list->desc());
      return error;
    }
  }

  return 0;
}

int MakeBound(BBox& b, const Transform& t, const Vec3 pt_list[], int pt_count)
{
  for (int i = 0; i < pt_count; ++i) {
    b.fit(MultPoint(pt_list[i], t.global));
  }

  return 0;
}
