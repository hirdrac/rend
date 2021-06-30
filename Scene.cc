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
// Member Functions
void Scene::clear()
{
  ambient = nullptr;
  background = nullptr;
  air = nullptr;
  default_obj = nullptr;
  default_lt = nullptr;
  image_width = 256;
  image_height = 256;
  eye.set(0,0,1);
  coi.set(0,0,0);
  vup.set(0,1,0);
  fov = 50.0;
  index = 1.0;
  samples_x = 1;
  samples_y = 1;
  shadow = true;
  reflect = true;
  transmit = true;
  max_ray_depth = 99;
  min_ray_value = VERY_SMALL;

  // object clear
  lights.clear();
  _shaders.clear();
  object_list.purge();
  _bound.reset();
}

int Scene::add(SceneItem* i, SceneItemFlag flag)
{
  if (Object* ob = dynamic_cast<Object*>(i); ob != nullptr) {
    object_list.addToTail(ob);
    return 0;
  }

  if (Light* lt = dynamic_cast<Light*>(i); lt != nullptr) {
    lights.emplace_back(lt);
    return 0;
  }

  if (Shader* sh = dynamic_cast<Shader*>(i); sh != nullptr) {
    switch (flag) {
      case AIR:
        if (air) { return -1; } else { air = sh; }
        break;
      case AMBIENT:
        if (ambient) { return -1; } else { ambient = sh; }
        break;
      case BACKGROUND:
        if (background) { return -1; } else { background = sh; }
        break;
      case DEFAULT_LT:
        if (default_lt) { return -1; } else { default_lt = sh; }
        break;
      case DEFAULT_OBJ:
        if (default_obj) { return -1; } else { default_obj = sh; }
        break;
      default:
        // object/light shaders are added to the scene
        // (which is in charge of initialization & memory)
        break;
    }
    _shaders.emplace_back(sh);
    return 0;
  }

  return -1; // failed to add
}

int Scene::init()
{
  // Set default scene shaders
  if (!ambient) {
    ambient = new ShaderColor( .1,  .1,  .1);
    _shaders.emplace_back(ambient);
  }

  if (!background) {
    background = new ShaderColor( .2,  .2,  .5);
    _shaders.emplace_back(background);
  }

  if (!default_obj) {
    default_obj = new ShaderColor( .3,  .3,  .3);
    _shaders.emplace_back(default_obj);
  }

  if (!default_lt) {
    default_lt = new ShaderColor(1.0, 1.0, 1.0);
    _shaders.emplace_back(default_lt);
  }

  // Init scene items
  if (InitObjectList(*this, object_list.head(), default_obj)) {
    println("Error initializing object list");
    return -1;  // error
  }

  // setup bounding boxes
  _bound.reset(MakeBoundList(object_list.head()));
  assert(!_bound || _bound->next() == nullptr);

  // init lights
  ShadowFn sFn = shadow ? CastShadow : CastNoShadow;
  for (auto& lt : lights) {
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
  for (auto& lt : lights) { println_os(out, "  ", lt->desc()); }
  println_os(out);
}

int Scene::traceRay(const Ray& r, Color& result) const
{
  ++r.stats->rays_cast;

  const Object* o_list = _bound.get();
  if (!o_list) { o_list = object_list.head(); }

  HitList hit_list(r.freeCache);
  for (const Object* ob = o_list; ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hit_list);
  }

  HitInfo* hit = hit_list.findFirstHit(r);
  const Object* obj = hit ? hit->object : nullptr;
  if (!obj) {
    // hit background
    HitInfo h(nullptr, VERY_LARGE, {0,0,0});
    Vec3 map{(r.dir.z > 0.0) ? r.dir.x : -r.dir.x, r.dir.y, 0.0};
    background->evaluate(*this, r, h, {0,0,1}, map, result);
    return 0;
  }

  ++r.stats->rays_hit;
  Vec3 normal{}, map{};
  obj->evalHit(*hit, normal, map);
  if (DotProduct(r.dir, normal) > 0.0) { normal.invert(); }

  Shader* sh = obj->shader();
  if (!sh) {
    sh = default_obj;
    if (!sh) {
      println("no shader!!");
      result.clear();
      return 0;
    }
  }

  hit->global_pt = CalcHitPoint(r.base, r.dir, hit->distance);
  sh->evaluate(*this, r, *hit, normal, map, result);
  return 0;
}

int Scene::traceShadowRay(const Ray& r, Color& result) const
{
  ++r.stats->shadow_rays_cast;

  const Object* o_list = _bound.get();
  if (!o_list) { o_list = object_list.head(); }

  HitList hit_list(r.freeCache);
  for (const Object* ob = o_list; ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hit_list);
  }

  HitInfo* hit = hit_list.findFirstHit(r);
  if (hit) {
    ++r.stats->shadow_rays_hit;
    result.clear(); // transparency not supported
  } else {
    result.full();
  }

  return 0;
}
