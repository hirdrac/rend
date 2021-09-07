//
// ColorShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "ColorShaders.hh"
#include "Ray.hh"


// **** ColorCube Class ****
Color ColorCube::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  return {
    static_cast<Color::value_type>(Abs(m.x)),
    static_cast<Color::value_type>(Abs(m.y)),
    static_cast<Color::value_type>(Abs(m.z))
  };
}
