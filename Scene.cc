//
// Scene.cc
// Copyright (C) 2020 Richard Bradley
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
#include "Parse.hh"
#include "Stats.hh"
#include "Color.hh"
#include "BasicShaders.hh"
#include <iostream>
#include <vector>


/**** Scene Class ****/
// Constructor
Scene::Scene() :
  ambient(nullptr), background(nullptr), air(nullptr),
  default_obj(nullptr), default_lt(nullptr),
  image_width(256), image_height(256),
  eye(0,0,1), coi(0,0,0), vup(0,1,0), fov(50), index(1.0),
  samples_x(1), samples_y(1),
  shadow(true), reflect(true), transmit(true),
  max_ray_depth(99), min_ray_value(VERY_SMALL)
{ }

// Destructor
Scene::~Scene()
{
  for (Light* lt : lights) { delete lt; }
  for (Shader* sh : _shaders) { delete sh; }
}

// Member Functions
int Scene::generate(SceneDesc& sd)
{
  int errors = ProcessList(*this, nullptr, sd.node_list.head());
  if (!errors) {
    return 0;
  }

  std::cout << "errors detected: " << errors << '\n';
  return errors;
}

int Scene::add(SceneItem* i, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(i);
  if (ob) {
    object_list.addToTail(ob);
    return 0;
  }

  Light* lt = dynamic_cast<Light*>(i);
  if (lt) {
    lights.push_back(lt);
    return 0;
  }

  Shader* sh = dynamic_cast<Shader*>(i);
  if (sh) {
    _shaders.push_back(sh);
    switch (flag) {
      case AIR:         air = sh; break;
      case AMBIENT:     ambient = sh; break;
      case BACKGROUND:  background = sh; break;
      case DEFAULT_LT:  default_lt = sh; break;
      case DEFAULT_OBJ: default_obj = sh; break;
      default: break;
    }
    return 0;
  }

  return -1; // failed to add
}

int Scene::init()
{
  // Set default scene shaders
  if (!ambient) {
    ambient = new ShaderColor( .1,  .1,  .1);
    _shaders.push_back(ambient);
  }

  if (!background) {
    background = new ShaderColor( .2,  .2,  .5);
    _shaders.push_back(background);
  }

  if (!default_obj) {
    default_obj = new ShaderColor( .3,  .3,  .3);
    _shaders.push_back(default_obj);
  }

  if (!default_lt) {
    default_lt = new ShaderColor(1.0, 1.0, 1.0);
    _shaders.push_back(default_lt);
  }

  // Init scene items
  if (InitObjectList(*this, object_list.head(), default_obj)) {
    std::cout << "Error initializing object list\n";
    return -1;  // error
  }

  // setup bounding boxes
  bound_list.purge();
  bound_list.addToTail(MakeBoundList(object_list.head()));

  // init lights
  ShadowFn sFn = shadow ? CastShadow : CastNoShadow;
  for (Light* lt : lights) {
    if (InitLight(*this, *lt, sFn)) {
      std::cout << "Error initializing light list\n";
      return -1;  // error
    }
  }

  // init shaders
  for (Shader* sh : _shaders) {
    if (InitShader(*this, *sh)) {
      std::cout << "Error initializing shaders\n";
      return -1;
    }
  }

  // everything okay
  return 0;
}

void Scene::info(std::ostream& out) const
{
  out <<   "    Image size:\t" << image_width << " x " << image_height;
  out << "\n           Fov:\t" << fov;
  out << "\n       Eye/Coi:\t" << eye << " / " << coi;
  out << "\n    VUP vector:\t" << vup;
  out << "\n Max ray depth:\t" << max_ray_depth;
  out << "\n Min ray value:\t" << min_ray_value;
  //out << "\nAmbient shader:\n";
  //  PrintBranch(ambient, out, 1);
  //out << "Background shader:\n";
  //  PrintBranch(background, out, 1);

  out << "\nLight List:\n";
  PrintList2(out, lights, 1);
  out << '\n';
}

int Scene::traceRay(const Ray& r, Color& result) const
{
  ++r.stats->rays_cast;

  const Object* o_list = bound_list.head();
  if (!o_list) { o_list = object_list.head(); }

  HitList hit_list(r.freeCache);
  for (const Object* ob = o_list; ob != nullptr; ob = ob->next()) {
    ob->intersect(r, hit_list);
  }

  HitInfo* hit = hit_list.findFirstHit(r);
  if (!hit || !hit->object) {
    // hit background
    HitInfo h(nullptr, VERY_LARGE, {0,0,0});
    Vec3 map{(r.dir.z > 0.0) ? r.dir.x : -r.dir.x, r.dir.y, 0.0};
    background->evaluate(*this, r, h, {0,0,1}, map, result);
    return 0;
  }

  ++r.stats->rays_hit;
  Vec3 normal{}, map{};
  hit->object->evalHit(*hit, normal, map);
  if (DotProduct(r.dir, normal) > 0.0) { normal.invert(); }

  Shader* sh = hit->object->shader();
  if (!sh) {
    sh = default_obj;
    if (!sh) {
      std::cout << "no shader!!\n";
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

  const Object* o_list = bound_list.head();
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
