//
// Transform.cc
// Copyright (C) 2021 Richard Bradley
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
  base.setIdentity();
  _final.setIdentity();
  _finalInv.setIdentity();
}
