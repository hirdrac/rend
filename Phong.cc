//
// Phong.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Phong.hh"
#include "Scene.hh"
#include "Light.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "BasicShaders.hh"


// **** Phong Class ****
int Phong::init(Scene& s)
{
  if (!_diffuse) { _diffuse = makeShader<ShaderColor>(.5, .5, .5); }
  if (int er = InitShader(s, *_diffuse); er != 0) { return er; }

  if (_specular) {
    if (int er = InitShader(s, *_specular); er != 0) { return er; }
  }

  if (_transmit) {
    if (int er = InitShader(s, *_transmit); er != 0) { return er; }
  }
  return 0;
}

int Phong::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh) { return -1; }

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
int Phong::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  const auto black_val = static_cast<Color::value_type>(s.min_ray_value);

  // Evaluate Shaders
  Color color_d;
  _diffuse->evaluate(s, r, h, eh, color_d);
  bool is_d = !color_d.isBlack(black_val);

  Color color_s;
  bool is_s;
  if (_specular) {
    _specular->evaluate(s, r, h, eh, color_s);
    is_s = !color_s.isBlack(black_val);
  } else {
    color_s = colors::black;
    is_s = false;
  }

#if 0
  Color color_t;
  bool is_t;
  if (_transmit) {
    _transmit->evaluate(s, r, h, eh, color_t);
    is_t = !color_t.isBlack(black_val);
  } else {
    color_t.clear();
    is_t = false;
  }
#endif

  Vec3 reflect = CalcReflect(r.dir, eh.normal);
  Color tmp;

  // Ambient calculations
  s.ambient->evaluate(s, r, h, eh, tmp);
  MultColor(tmp, color_d, result);

  for (auto& lt : s.lights()) {
    LightResult lresult;
    lt->luminate(s, r, h, eh, lresult);
    Flt angle = DotProduct(eh.normal, lresult.dir);
    if (!IsPositive(angle)) {
      continue;
    }

    lt->shadow_fn(s, r, eh.global_pt, lresult);

    // Diffuse calculations
    if (is_d) {
      MultColor(lresult.energy, color_d, tmp);
      tmp *= angle;
      result += tmp;
    }

    // Specular calculations
    if (is_s) {
      angle = DotProduct(reflect, lresult.dir);
      if (angle > 0.0) {
	Flt p = std::pow(angle, exp);  // specular hi-light
	MultColor(lresult.energy, color_s, tmp);
	tmp *= p;
	result += tmp;
      }
    }
  }

  if (is_s && r.depth < s.max_ray_depth) {
    // add in reflection
    Ray ray;
    ray.base       = eh.global_pt;
    ray.dir        = reflect;
    ray.min_length = s.ray_moveout;
    ray.max_length = VERY_LARGE;
    ray.time       = 0.0;
    ray.depth      = r.depth + 1;

    ray.freeCache = r.freeCache;
    ray.stats = r.stats;

    Color c;
    s.traceRay(ray, c);
    c *= color_s;
    result += c;
  }

  return 0;
}
