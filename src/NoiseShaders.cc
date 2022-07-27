//
// NoiseShaders.cc
// Copyright (C) 2022 Richard Bradley
//

#include "NoiseShaders.hh"
#include "PerlinNoise.hh"
#include "RegisterShader.hh"
#include <cassert>


// **** NoiseShader Class ****
REGISTER_SHADER_CLASS(NoiseShader,"noise");

int NoiseShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int NoiseShader::init(Scene& s, const Transform* tr)
{
  return _child ? InitShader(s, *_child, tr) : -1;
}

Color NoiseShader::evaluate(JobState& js, const Scene& s, const Ray& r,
                            const EvaluatedHit& eh) const
{
  const Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  EvaluatedHit eh2 = eh;
  eh2.map.x += perlin::noise<Flt>(m.x, m.y, m.z) * _value;
  return _child->evaluate(js, s, r, eh2);
}
