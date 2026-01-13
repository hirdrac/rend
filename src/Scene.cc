//
// Scene.cc
// Copyright (C) 2026 Richard Bradley
//

#include "Scene.hh"
#include "Light.hh"
#include "Object.hh"
#include "Shader.hh"
#include "Bound.hh"
#include "Ray.hh"
#include "Intersect.hh"
#include "JobState.hh"
#include "Color.hh"
#include "Print.hh"
#include <cassert>


// **** Scene Class ****
Scene::~Scene() = default;

void Scene::clear()
{
  ambient.reset();
  background.reset();
  _defaultObj.reset();
  _defaultLt.reset();
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
  aperture = 0.0;
  focus = 1;
  sample_x = 1;
  sample_y = 1;
  jitter = 0.0;
  samples = 1;
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
  assert(ob != nullptr);
  _objects.push_back(ob);
  return 0;
}

int Scene::addLight(const LightPtr& lt)
{
  assert(lt != nullptr);
  _lights.push_back(lt);
  return 0;
}

int Scene::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  switch (flag) {
    case FLAG_AMBIENT:
      if (ambient) { return -1; } else { ambient = sh; }
      break;
    case FLAG_BACKGROUND:
      if (background) { return -1; } else { background = sh; }
      break;
    case FLAG_DEFAULT_LT:
      if (_defaultLt) { return -1; } else { _defaultLt = sh; }
      break;
    case FLAG_DEFAULT_OBJ:
      if (_defaultObj) { return -1; } else { _defaultObj = sh; }
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

  if (!_defaultObj) {
    _defaultObj = makeShader<ShaderColor>( .3,  .3,  .3);
    _shaders.push_back(_defaultObj);
  }

  if (!_defaultLt) {
    _defaultLt = makeShader<ShaderColor>(1.0, 1.0, 1.0);
    _shaders.push_back(_defaultLt);
  }

  // init lights
  // (lights initialized before objects so lights contained in groups will
  //  be re-initialized with the correct parent transform)
  for (auto& lt : _lights) {
    if (initLight(*lt, nullptr)) {
      println("Error initializing light list");
      return -1;  // error
    }
  }

  // init objects
  csg_count = 0;
  group_count = 0;
  object_count = 0;
  for (auto& ob : _objects) {
    if (initObject(*ob, nullptr, nullptr)) {
      println("Error initializing object list");
      return -1;  // error
    }
  }

  // setup bounding boxes
  bound_count = makeBoundList(*this, _objects, _optObjects);

  // init shaders
  shader_count = 0;
  for (auto& sh : _shaders) {
    if (initShader(*sh, nullptr)) {
      println("Error initializing shaders");
      return -1;
    }
  }

  // everything okay
  return 0;
}

int Scene::initLight(Light& lt, const Transform* tr)
{
  Transform* trans = lt.trans();
  if (trans) { trans->init(tr); }

  if (!lt.energy()) { lt.addShader(_defaultLt, FLAG_NONE); }

  return lt.init(*this);
}

int Scene::initObject(Object& ob, const ShaderPtr& sh, const Transform* tr)
{
  Transform* trans = ob.trans();
  if (trans) { trans->init(tr); tr = trans; }

  // Assign provided shader if none is set
  if (sh && !ob.shader()) { ob.addShader(sh, FLAG_NONE); }

  ++object_count;
  const int error = ob.init(*this, tr);
  if (error) {
    println("INIT ERROR: ", ob.desc());
    return error;
  }

  return 0;
}

int Scene::initShader(Shader& sh, const Transform* tr)
{
  Transform* trans = sh.trans();
  if (trans) { trans->init(tr); tr = trans; }

  ++shader_count;
  return sh.init(*this, tr);
}

Color Scene::traceRay(JobState& js, const Ray& r) const
{
  StatInfo& si = js.stats;
  ++si.rays.tried;

  HitList hit_list{js.cache, si, false};
  for (auto& ob : _optObjects) { ob->intersect(r, hit_list); }

  const HitInfo* hit = hit_list.firstHit();
  if (!hit) {
    // hit background
    const EvaluatedHit eh{
      {}, {}, {(r.dir.z > 0.0) ? r.dir.x : -r.dir.x, r.dir.y, 0.0}, 0};
    return background->evaluate(js, *this, r, eh);
  }

  ++si.rays.hit;

  const Primitive* obj = hit->object;
  const Shader* sh = obj->shader().get();
  if (!sh) { sh = _defaultObj.get(); }

  EvaluatedHit eh{
    CalcHitPoint(r.base, r.dir, hit->distance),
    obj->normal(r, *hit),
    hit->local_pt,
    hit->side
  };
  if (dotProduct(r.dir, eh.normal) > 0.0) { eh.normal = -eh.normal; }

  return sh->evaluate(js, *this, r, eh);
}

bool Scene::castShadowRay(JobState& js, const Ray& r) const
{
  StatInfo& si = js.stats;
  ++si.shadow_rays.tried;

  HitList hit_list{js.cache, si, false};
  for (auto& ob : _optObjects) { ob->intersect(r, hit_list); }

  const HitInfo* hit = hit_list.firstHit();
  if (!hit) { return false; }

  ++si.shadow_rays.hit;
  // transparency not supported
  return true;
}
