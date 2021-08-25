//
// Phong.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Phong.hh"
#include "Scene.hh"
#include "Light.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include <cassert>


// **** Phong Class ****
int Phong::init(Scene& s)
{
  if (!_diffuse) { _diffuse = makeShader<ShaderColor>(.5, .5, .5); }
  if (int er = InitShader(s, *_diffuse); er != 0) { return er; }

  if (!_specular) { _specular = makeShader<ShaderColor>(0, 0, 0); }
  if (int er = InitShader(s, *_specular); er != 0) { return er; }

  if (!_transmit) { _transmit = makeShader<ShaderColor>(0, 0, 0); }
  if (int er = InitShader(s, *_transmit); er != 0) { return er; }

  return 0;
}

int Phong::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  switch (flag) {
    case FLAG_NONE:
    case FLAG_DIFFUSE:
      if (_diffuse) { return -1; }
      _diffuse = sh;
      break;

    case FLAG_SPECULAR:
      if (_specular) { return -1; }
      _specular = sh;
      break;

    case FLAG_TRANSMIT:
      if (_transmit) { return -1; }
      _transmit = sh;
      break;

    default:
      return -1;
  }

  return 0;
}

// Shader Functions
Color Phong::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  const auto black_val = static_cast<Color::value_type>(s.min_ray_value);

  // Evaluate Shaders
  const Color color_d = _diffuse->evaluate(s, r, h, eh);
  const bool is_d = !color_d.isBlack(black_val);

  const Color color_s = _specular->evaluate(s, r, h, eh);
  const bool is_s = !color_s.isBlack(black_val);

#if 0
  const Color color_t = _transmit->evaluate(s, r, h, eh);
  const bool is_t = !color_t.isBlack(black_val);
#endif

  Vec3 reflect;
  if (is_s) {
    Vec3 dir = r.dir;
    // init ray not normalized
    if (r.depth == 0) { dir.normalize(); }

    //const Flt len_sqr = dir.lengthSqr();
    //if (!IsOne(len_sqr)) { dir *= 1.0 / std::sqrt(len_sqr); }

    reflect = CalcReflect(dir, eh.normal);
  }

  Ray ray;
  ray.base       = eh.global_pt;
  ray.min_length = s.ray_moveout;
  ray.time       = r.time;
  ray.depth      = r.depth + 1;
  ray.freeCache  = r.freeCache;
  ray.stats      = r.stats;

  // ambient calculation
  Color result = s.ambient->evaluate(s, r, h, eh) * color_d;

  for (auto& lt : s.lights()) {
    LightResult lresult;
    lt->luminate(s, r, h, eh, lresult);
    Flt angle = DotProduct(eh.normal, lresult.dir);
    if (!IsPositive(angle)) { continue; }

    if (s.shadow) {
      // shadow check
      ray.dir        = lresult.dir;
      ray.max_length = lresult.distance;

      lresult.energy *= s.traceShadowRay(ray);
    }

    if (is_d) {
      // diffuse calculation
      result += (lresult.energy * color_d) * angle;
    }

    if (is_s) {
      // specular hi-light calculation
      angle = DotProduct(reflect, lresult.dir);
      if (angle > 0.0) {
	result += (lresult.energy * color_s) * std::pow(angle, exp);
      }
    }
  }

  if (is_s && r.depth < s.max_ray_depth) {
    // add in reflection
    ray.dir        = reflect;
    ray.max_length = VERY_LARGE;

    result += s.traceRay(ray) * color_s;
  }

  return result;
}
