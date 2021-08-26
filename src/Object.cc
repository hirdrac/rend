//
// Object.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of object classes
//

#include "Object.hh"
#include "Shader.hh"
#include "Scene.hh"
#include "Stats.hh"
#include "BBox.hh"
#include "Print.hh"
#include <cassert>


// **** Object Class ****
const std::vector<ObjectPtr> Object::_emptyList;


// **** Primitive Class ****
int Primitive::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_shader) { return -1; }

  _shader = sh;
  return 0;
}

static constexpr Vec3 unitCubePoints[8] = {
  { 1, 1, 1}, {-1, 1, 1}, { 1,-1, 1}, { 1, 1,-1},
  {-1,-1, 1}, { 1,-1,-1}, {-1, 1,-1}, {-1,-1,-1}};

BBox Primitive::bound() const
{
  return BBox(unitCubePoints, std::size(unitCubePoints), _trans);
}

BBox Primitive::localBound() const
{
  return BBox(unitCubePoints, std::size(unitCubePoints));
}


// **** Functions ****
int InitObject(Scene& s, Object& ob, const ShaderPtr& sh, const Transform* t)
{
  Transform* trans = ob.trans();
  if (trans) { trans->init(t); }

  // Assign provided shader if none is set
  if (sh && !ob.shader()) { ob.addShader(sh, FLAG_NONE); }

  ++s.object_count;
  int error = ob.init(s);
  if (error) {
    println("INIT ERROR: ", ob.desc());
    return error;
  }

  return 0;
}

int InitObjectOnlyTransforms(Object& ob, const Transform* t)
{
  Transform* trans = ob.trans();
  if (trans) { trans->init(t); }

  for (auto& child : ob.children()) {
    InitObjectOnlyTransforms(*child, trans);
  }

  return 0;
}
