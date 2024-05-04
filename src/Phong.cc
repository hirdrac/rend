//
// Phong.cc
// Copyright (C) 2024 Richard Bradley
//

#include "Phong.hh"
#include "Scene.hh"
#include "Light.hh"
#include "Ray.hh"
#include "RegisterShader.hh"
#include <cassert>


// **** Phong Class ****
REGISTER_SHADER_CLASS(Phong,"phong");

int Phong::init(Scene& s, const Transform* tr)
{
  if (!_ambient) { _ambient = s.ambient; }
  else if (int er = s.initShader(*_ambient, tr); er != 0) { return er; }

  if (!_diffuse) { _diffuse = makeShader<ShaderColor>(.5, .5, .5); }
  if (int er = s.initShader(*_diffuse, tr); er != 0) { return er; }

  if (!_specular) { _specular = makeShader<ShaderColor>(0, 0, 0); }
  if (int er = s.initShader(*_specular, tr); er != 0) { return er; }

  if (!_transmit) { _transmit = makeShader<ShaderColor>(0, 0, 0); }
  if (int er = s.initShader(*_transmit, tr); er != 0) { return er; }

  return 0;
}

int Phong::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  switch (flag) {
    case FLAG_AMBIENT:
      if (_ambient) { return -1; }
      _ambient = sh;
      break;

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
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const auto black_val = static_cast<Color::value_type>(s.min_ray_value);

  // Evaluate Shaders
  const Color color_d = _diffuse->evaluate(js, s, r, eh);

  const Color color_s = _specular->evaluate(js, s, r, eh);
  const bool is_s = !color_s.isBlack(black_val);

#if 0
  const Color color_t = _transmit->evaluate(js, s, r, eh);
  const bool is_t = !color_t.isBlack(black_val);
#endif

  Vec3 reflect{INIT_NONE};
  if (is_s) { reflect = CalcReflect(r.dir, eh.normal); }

  // ambient calculation
  Color result = _ambient->evaluate(js, s, r, eh) * color_d;

  // diffuse/specular lighting calculations
  for (auto& lt : s.lights()) {
    LightResult lresult;
    if (!lt->luminate(js, s, r, eh, lresult)) { continue; }

    // diffuse calculation
    result += (lresult.energy * color_d) * lresult.angle;

    if (is_s) {
      // specular hi-light calculation
#if 1
      // phong
      const Flt angle = DotProduct(reflect, lresult.dir);
      if (IsPositive(angle)) {
	result += (lresult.energy * color_s) * std::pow(angle, exp);
      }
#else
      // blinn-phong
      const Vec3 halfway = UnitVec(lresult.dir - r.dir);
      const Flt angle = DotProduct(halfway, eh.normal);
      if (IsPositive(angle)) {
	result += (lresult.energy * color_s) * std::pow(angle, exp * 4.0);
      }
#endif
    }
  }

  // reflection calculation
  if (is_s && r.depth < s.max_ray_depth) {
    Ray ray {
      .base       = eh.global_pt,
      .dir        = reflect,
      .min_length = s.ray_moveout,
      .max_length = VERY_LARGE,
      .time       = r.time,
      .depth      = r.depth + 1
    };

    result += s.traceRay(js, ray) * color_s;
  }

  return result;
}
