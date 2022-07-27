//
// Occlusion.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Occlusion.hh"
#include "JobState.hh"
#include "Scene.hh"
#include "Ray.hh"
#include "RegisterShader.hh"
#include <cassert>


// **** Occlusion Class ****
REGISTER_SHADER_CLASS(Occlusion,"occlusion");

int Occlusion::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (_child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int Occlusion::init(Scene& s, const Transform* tr)
{
  return _child ? InitShader(s, *_child, tr) : -1;
}

Color Occlusion::evaluate(JobState& js, const Scene& s, const Ray& r,
                          const EvaluatedHit& eh) const
{
  Ray sray;
  sray.base       = eh.global_pt;
  sray.min_length = s.ray_moveout;
  sray.max_length = _radius;
  sray.time       = r.time;
  sray.depth      = 0;

  int lit = 0;
  for (int i = 0; i < _samples; ++i) {
    sray.dir = js.rnd.hemisphereDir(eh.normal);
    if (!s.castShadowRay(js, sray)) { ++lit; }
  }

  if (lit == 0) {
    return colors::black;
  }

  return _child->evaluate(js, s, r, eh)
    * (Color::value_type(lit) / Color::value_type(_samples));
}
