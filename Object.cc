//
// Object.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of object classes
//

#include "Object.hh"
#include "Shader.hh"
#include "Stats.hh"
#include "BBox.hh"
#include "Print.hh"


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

  // Assign provided shader if none is set
  if (sh && !ob.shader()) { ob.addShader(sh, FLAG_NONE); }

  // Init primitive
  int error = ob.init(s);
  if (error) {
    println("INIT ERROR: ", ob.desc());
    return error;
  }

  return 0;
}
