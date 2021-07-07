//
// Phong.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of the phong illumination class
//

#include "Phong.hh"
#include "Scene.hh"
#include "Light.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "BasicShaders.hh"


// **** Phong Class ****
// SceneItem Functions
int Phong::init(Scene& s)
{
  if (!_diffuse) { _diffuse = std::make_unique<ShaderColor>(.5, .5, .5); }
  if (int er = InitShader(s, *_diffuse, value); er != 0) { return er; }

  if (_specular) {
    if (int er = InitShader(s, *_specular, value); er != 0) { return er; }
  }

  if (_transmit) {
    if (int er = InitShader(s, *_transmit, value); er != 0) { return er; }
  }
  return 0;
}

int Phong::addShader(Shader* sh, SceneItemFlag flag)
{
  if (!sh) { return -1; }

  switch (flag) {
    default:
    case DIFFUSE:
      if (_diffuse) { return -1; }
      _diffuse.reset(sh);
      break;

    case SPECULAR:
      if (_specular) { return -1; }
      _specular.reset(sh);
      break;

    case TRANSMIT:
      if (_transmit) { return -1; }
      _transmit.reset(sh);
      break;
  }

  return 0;
}

// Shader Functions
int Phong::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  // Evaluate Shaders
  Color color_d;
  _diffuse->evaluate(s, r, h, normal, map, color_d);
  bool is_d = !color_d.isBlack(s.min_ray_value);

  Color color_s;
  bool is_s;
  if (_specular) {
    _specular->evaluate(s, r, h, normal, map, color_s);
    is_s = !color_s.isBlack(s.min_ray_value);
  } else {
    color_s.clear();
    is_s = false;
  }

#if 0
  Color color_t;
  bool is_t;
  if (_transmit) {
    _transmit->evaluate(s, r, h, normal, map, color_t);
    is_t = !color_t.isBlack(s.min_ray_value);
  } else {
    color_t.clear();
    is_t = false;
  }
#endif

  Vec3 reflect = CalcReflect(r.dir, normal);
  Color tmp;

  // Ambient calculations
  s.ambient->evaluate(s, r, h, normal, map, tmp);
  MultColor(tmp, color_d, result);

  for (auto& lt : s.lights) {
    LightResult lresult;
    lt->luminate(s, r, h, normal, map, lresult);
    Flt angle = DotProduct(normal, lresult.dir);
    if (!IsPositive(angle)) {
      continue;
    }

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
    ray.base = h.global_pt;
    ray.dir = reflect;
    ray.max_length = VERY_LARGE;
    ray.time = 0.0;
    ray.depth = r.depth + 1;
    ray.moveOut();

    ray.freeCache = r.freeCache;
    ray.stats = r.stats;

    Color c;
    s.traceRay(ray, c);
    c *= color_s;
    result += c;
  }

  return 0;
}
