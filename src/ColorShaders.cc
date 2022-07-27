//
// ColorShaders.cc
// Copyright (C) 2022 Richard Bradley
//

#include "ColorShaders.hh"
#include "Ray.hh"
#include "RegisterShader.hh"


// **** ColorCube Class ****
REGISTER_SHADER_CLASS(ColorCube,"colorcube");

Color ColorCube::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  return {
    static_cast<Color::value_type>(Abs(m.x)),
    static_cast<Color::value_type>(Abs(m.y)),
    static_cast<Color::value_type>(Abs(m.z))
  };
}
