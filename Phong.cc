//
// Phong.cc - revision 19 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//
// Implementation of the phong illumination class
//

#include "Phong.hh"
#include "Scene.hh"
#include "Light.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Bound.hh"
#include "BasicShaders.hh"
#include <iostream>


// ---- Phong Class ----
// Destructor
Phong::~Phong()
{
  delete diffuse;
  delete specular;
  delete transmit;
}

// SceneItem Functions
int Phong::init(Scene& s)
{
  int error = 0;
  if (!diffuse) {
    diffuse = new ShaderColor(.5, .5, .5);
  }

  if (!specular) {
    specular = new ShaderColor(0, 0, 0);
  }

  if (!transmit) {
    transmit = new ShaderColor(0, 0, 0);
  }

  error += Shader::init(s);
  error += InitShader(s, *diffuse, value);
  error += InitShader(s, *specular, value);
  error += InitShader(s, *transmit, value);
  return error;
}

int Phong::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh) {
    return Shader::add(i, flag);
  }

  switch (flag) {
    default:
    case DIFFUSE:
      delete diffuse;
      diffuse = sh;
      break;

    case SPECULAR:
      delete specular;
      specular = sh;
      break;

    case TRANSMIT:
      delete transmit;
      transmit = sh;
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
  diffuse->evaluate(s, r, h, normal, map, color_d);
  bool is_d = !color_d.isBlack(s.min_ray_value);

  Color color_s;
  specular->evaluate(s, r, h, normal, map, color_s);
  bool is_s = !color_s.isBlack(s.min_ray_value);

#if 0
  Color color_t;
  transmit->evaluate(s, r, h, normal, map, color_t);
  bool is_t = !color_t.isBlack(s.min_ray_value);
#endif

  Vec3 reflect = CalcReflect(r.dir, normal);

  Color tmp;

  // Ambient calculations
  s.ambient->evaluate(s, r, h, normal, map, tmp);
  MultColor(tmp, color_d, result);

  for (Light* l : s.lights) {
    LightResult lresult;
    l->luminate(s, r, h, normal, map, lresult);
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

    Color c;
    s.traceRay(ray, c);
    c *= color_s;
    result += c;
  }

  return 0;
}
