//
// Transform.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of motion module
//

#include "Transform.hh"


/**** Transform Class ****/
// Member Functions
int Transform::init()
{
  if (InvertMatrix(global, global_inv)) {
    return -1;
  }

  return 0;
}

void Transform::clear()
{
  local.setIdentity();
  global.setIdentity();
  global_inv.setIdentity();
}


/**** Functions ****/
std::ostream& operator<<(std::ostream& out, const Transform& t)
{
  return out << t.local << "--\n" << t.global << "--\n" << t.global_inv;
}
