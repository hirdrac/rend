//
// Transform.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Transform.hh"


// **** Transform Class ****
int Transform::init()
{
  if (InvertMatrix(global, _globalInv)) {
    return -1;
  }

  return 0;
}

void Transform::clear()
{
  local.setIdentity();
  global.setIdentity();
  _globalInv.setIdentity();
}
