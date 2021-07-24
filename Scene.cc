//
// Scene.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of scene class
//

#include "Scene.hh"
#include "Light.hh"
#include "Object.hh"
#include "Shader.hh"
#include "Bound.hh"
#include "Ray.hh"
#include "Intersect.hh"
#include "Stats.hh"
#include "Color.hh"
#include "BasicShaders.hh"
#include "Print.hh"
#include <vector>
#include <cassert>


// **** Scene Class ****
Scene::~Scene() = default;

void Scene::clear()
{
  ambient.reset();
  background.reset();
  air.reset();
  default_obj.reset();
  default_lt.reset();
  image_width = 256;
  image_height = 256;
  eye = {0,0,1};
  coi = {0,0,0};
  vup = {0,1,0};
  fov = 50.0;
  samples_x = 1;
  samples_y = 1;
  jitter = 0.0;
  shadow = true;
  reflect = true;
  transmit = true;
  max_ray_depth = 99;
  min_ray_value = VERY_SMALL;
  ray_moveout = .00001;

  // object clear
  _objects.clear();
  _bound.reset();
  _lights.clear();
  _shaders.clear();
}

int Scene::addObject(const ObjectPtr& ob)
{
  if (!ob) { return -1; }

  _objects.push_back(ob);
  return 0;
}

int Scene::addLight(const LightPtr& lt)
{
  if (!lt) { return -1; }

  _lights.push_back(lt);
  return 0;
}

int Scene::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh) { return -1; }

  switch (flag) {
    case FLAG_AIR:
      if (air) { return -1; } else { air = sh; }
      break;
    case FLAG_AMBIENT:
      if (ambient) { return -1; } else { ambient = sh; }
      break;
    case FLAG_BACKGROUND:
      if (background) { return -1; } else { background = sh; }
      break;
    case FLAG_DEFAULT_LT:
      if (default_lt) { return -1; } else { default_lt = sh; }
      break;
    case FLAG_DEFAULT_OBJ:
      if (default_obj) { return -1; } else { default_obj = sh; }
      break;
    case FLAG_INIT_ONLY:
      break;
      // object/light shaders are added to the scene
      // (which is in charge of initialization)
    default:
      return -1;
  }
  _shaders.push_back(sh);
  return 0;
}

int Scene::init()
{
  // Set default scene shaders
  if (!ambient) {
    ambient = makeShader<ShaderColor>( .1,  .1,  .1);
    _shaders.push_back(ambient);
  }

  if (!background) {
    background = makeShader<ShaderColor>( .2,  .2,  .5);
    _shaders.push_back(background);
  }

  if (!default_obj) {
    default_obj = makeShader<ShaderColor>( .3,  .3,  .3);
    _shaders.push_back(default_obj);
  }

  if (!default_lt) {
    default_lt = makeShader<ShaderColor>(1.0, 1.0, 1.0);
    _shaders.push_back(default_lt);
  }

  // Init scene items
  for (auto& ob : _objects) {
    if (InitObject(*this, *ob, default_obj)) {
      println("Error initializing object list");
      return -1;  // error
    }
  }

  // setup bounding boxes
  _bound = MakeBoundList(_objects);

  // init lights
  ShadowFn sFn = shadow ? CastShadow : CastNoShadow;
  for (auto& lt : _lights) {
    if (InitLight(*this, *lt, sFn)) {
      println("Error initializing light list");
      return -1;  // error
    }
  }

  // init shaders
  for (auto& sh : _shaders) {
    if (InitShader(*this, *sh)) {
      println("Error initializing shaders");
      return -1;
    }
  }

  // everything okay
  return 0;
}

void Scene::info(std::ostream& out) const
{
  println_os(out, "    Image size:\t", image_width, " x ", image_height);
  println_os(out, "           Fov:\t", fov);
  println_os(out, "       Eye/Coi:\t", eye, " / ", coi);
  println_os(out, "    VUP vector:\t", vup);
  println_os(out, " Max ray depth:\t", max_ray_depth);
  println_os(out, " Min ray value:\t", min_ray_value);
  println_os(out, "Light List:");
  for (auto& lt : _lights) { println_os(out, "  ", lt->desc()); }
  println_os(out);
}

int Scene::traceRay(const Ray& r, Color& result) const
{
  ++r.stats->rays.tried;

  HitList hit_list(r.freeCache);
  if (_bound) {
    _bound->intersect(r, false, hit_list);
  } else {
    for (auto& ob : _objects) { ob->intersect(r, false, hit_list); }
  }

  HitInfo* hit = hit_list.findFirstHit(r);
  EvaluatedHit eh{};

  const Object* obj = hit ? hit->object : nullptr;
  if (!obj) {
    // hit background
    HitInfo h(nullptr, VERY_LARGE, {0,0,0});
    eh.normal = {0,0,1};
    eh.map = {(r.dir.z > 0.0) ? r.dir.x : -r.dir.x, r.dir.y, 0.0};
    background->evaluate(*this, r, h, eh, result);
    return 0;
  }

  ++r.stats->rays.hit;
  obj->evalHit(*hit, eh);
  if (DotProduct(r.dir, eh.normal) > 0.0) { eh.normal.invert(); }

  Shader* sh = obj->shader().get();
  if (!sh) {
    sh = default_obj.get();
    if (!sh) {
      println("no shader!!");
      result.clear();
      return 0;
    }
  }

  eh.global_pt = CalcHitPoint(r.base, r.dir, hit->distance);
  sh->evaluate(*this, r, *hit, eh, result);
  return 0;
}

int Scene::traceShadowRay(const Ray& r, Color& result) const
{
  ++r.stats->shadow_rays.tried;

  HitList hit_list(r.freeCache);
  if (_bound) {
    _bound->intersect(r, false, hit_list);
  } else {
    for (auto& ob : _objects) { ob->intersect(r, false, hit_list); }
  }

  const HitInfo* hit = hit_list.findFirstHit(r);
  if (hit) {
    ++r.stats->shadow_rays.hit;
    result.clear(); // transparency not supported
  } else {
    result.full();
  }

  return 0;
}
