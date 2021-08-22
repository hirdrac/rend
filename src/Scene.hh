//
// Scene.hh
// Copyright (C) 2021 Richard Bradley
//
// Definition of scene class
//

#pragma once
#include "ObjectPtr.hh"
#include "LightPtr.hh"
#include "ShaderPtr.hh"
#include "SceneItem.hh"
#include "Types.hh"
#include <vector>


// **** Types ****
class SceneItem;
class Color;
class Ray;


// Scene class
//  storage class for all scene information
class Scene
{
 public:
  // Scene color shaders
  ShaderPtr ambient;
  ShaderPtr background;

  // Default shaders
  ShaderPtr default_obj;
  ShaderPtr default_lt;

  int  image_width, image_height;     // Image pixel size
  int  region_min[2], region_max[2];  // Render region

  // camera settings
  Vec3 eye;                   // camera location
  Vec3 coi;                   // center-of-interest
  Vec3 vup;                   // view-up vector
  Flt  fov;                   // field of view angle

  // anti-aliasing
  int  samples_x, samples_y;  // sub-pixel grid size
  Flt  jitter;                // x/y jitter amount for a sub-pixel
  int  jitter_count;          // sample count for a sub-pixel if jitter > 0

  // secondary ray settings
  bool shadow, reflect, transmit;
  int  max_ray_depth;
  Flt  min_ray_value;
  Flt  ray_moveout;

  // scene inventory count
  int bound_count;
  int csg_count;
  int group_count;
  int object_count;
  int shader_count;


  Scene() { clear(); }
  ~Scene();

  // Member Functions
  void clear();
  int addObject(const ObjectPtr& ob);
  int addLight(const LightPtr& lt);
  int addShader(const ShaderPtr& sh, SceneItemFlag flag);
  int init();

  Color traceRay(const Ray& r) const;
  Color traceShadowRay(const Ray& r) const;

  const std::vector<ObjectPtr>& objects() const { return _objects; }
  const std::vector<ObjectPtr>& optObjects() const { return _optObjects; }
  const std::vector<LightPtr>& lights() const { return _lights; }

 private:
  std::vector<ObjectPtr> _objects;
    // Complete list of objects (including Groups but not Bounds)

  std::vector<ObjectPtr> _optObjects;
    // bounding box optimized objects

  std::vector<LightPtr> _lights;

  std::vector<ShaderPtr> _shaders;
    // all top-level shader objects (for initialization)
};
