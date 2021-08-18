//
// Scene.cc
// Copyright (C) 2021 Richard Bradley
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
  region_min[0] = 0;
  region_min[1] = 0;
  region_max[0] = image_width - 1;
  region_max[1] = image_height - 1;
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
  ray_moveout = .0001;

  // object clear
  _objects.clear();
  _optObjects.clear();
  _lights.clear();
  _shaders.clear();

  bound_count = 0;
  group_count = 0;
  csg_count = 0;
  object_count = 0;
  shader_count = 0;
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
  csg_count = 0;
  group_count = 0;
  object_count = 0;
  for (auto& ob : _objects) {
    if (InitObject(*this, *ob, nullptr)) {
      println("Error initializing object list");
      return -1;  // error
    }
  }

  // setup bounding boxes
  bound_count = MakeBoundList(eye, _objects, _optObjects);

  // init lights
  for (auto& lt : _lights) {
    if (InitLight(*this, *lt)) {
      println("Error initializing light list");
      return -1;  // error
    }
  }

  // init shaders
  shader_count = 0;
  for (auto& sh : _shaders) {
    if (InitShader(*this, *sh)) {
      println("Error initializing shaders");
      return -1;
    }
  }

  // everything okay
  return 0;
}

Color Scene::traceRay(const Ray& r) const
{
  ++r.stats->rays.tried;

  HitList hit_list(r.freeCache, false);
  for (auto& ob : _optObjects) { ob->intersect(r, hit_list); }

  HitInfo* hit = hit_list.findFirstHit(r);
  if (!hit) {
    // hit background
    const HitInfo h{nullptr, VERY_LARGE, {}};
    const EvaluatedHit eh{
      {}, {0,0,1}, {(r.dir.z > 0.0) ? r.dir.x : -r.dir.x, r.dir.y, 0.0}};
    return background->evaluate(*this, r, h, eh);
  }

  ++r.stats->rays.hit;

  const Object* obj = hit->object;
  const Shader* sh = obj->shader().get();
  if (!sh) {
    if (hit->child) { sh = hit->child->shader().get(); }
    if (!sh) {
      sh = default_obj.get();
      if (!sh) {
        return colors::black;
      }
    }
  }

  EvaluatedHit eh{};
  eh.global_pt = CalcHitPoint(r.base, r.dir, hit->distance);
  obj->evalHit(r, *hit, eh);
  if (DotProduct(r.dir, eh.normal) > 0.0) { eh.normal = -eh.normal; }

  return sh->evaluate(*this, r, *hit, eh);
}

Color Scene::traceShadowRay(const Ray& r) const
{
  ++r.stats->shadow_rays.tried;

  HitList hit_list(r.freeCache, false);
  for (auto& ob : _optObjects) { ob->intersect(r, hit_list); }

  const HitInfo* hit = hit_list.findFirstHit(r);
  if (!hit) { return colors::white; }

  ++r.stats->shadow_rays.hit;
  return colors::black; // transparency not supported
}
