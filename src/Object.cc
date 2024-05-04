//
// Object.cc
// Copyright (C) 2024 Richard Bradley
//

#include "Object.hh"
#include "BBox.hh"
#include <cassert>


// **** Object Class ****
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
  return {std::data(pt), std::size(pt), t ? *t : _trans.final()};
}
