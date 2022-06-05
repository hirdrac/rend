//
// Transform.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Transform.hh"


// **** Transform Class ****
int Transform::init(const Transform* parent)
{
  _final = base;
  if (parent && !_noParent) {
    _final *= parent->final();
  }

  if (InvertMatrix(_final, _finalInv)) {
    return -1;
  }

  return 0;
}

void Transform::clear()
{
  base = INIT_IDENTITY;
  _final = INIT_IDENTITY;
  _finalInv = INIT_IDENTITY;
}
