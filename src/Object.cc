//
// Object.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Object.hh"
#include "Scene.hh"
#include "BBox.hh"
#include "Print.hh"
#include <cassert>


// **** Object Class ****
const std::vector<ObjectPtr> Object::_emptyList;

int Object::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_shader || flag != FLAG_NONE) { return -1; }

  _shader = sh;
  return 0;
}


// **** Primitive Class ****
BBox Primitive::bound(const Matrix* t) const
{
  static constexpr Vec3 pt[8] = {
    { 1, 1, 1}, {-1, 1, 1}, { 1,-1, 1}, { 1, 1,-1},
    {-1,-1, 1}, { 1,-1,-1}, {-1, 1,-1}, {-1,-1,-1}};
  return {pt, std::size(pt), t ? *t : _trans.final()};
}


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh, const Transform* tr)
{
  Transform* trans = ob.trans();
  if (trans) { trans->init(tr); tr = trans; }

  // Assign provided shader if none is set
  if (sh && !ob.shader()) { ob.addShader(sh, FLAG_NONE); }

  ++s.object_count;
  int error = ob.init(s, tr);
  if (error) {
    println("INIT ERROR: ", ob.desc());
    return error;
  }

  return 0;
}
