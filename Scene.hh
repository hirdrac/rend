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
  ShaderPtr air;

  // Default shaders
  ShaderPtr default_obj;
  ShaderPtr default_lt;

  int  image_width, image_height;     // Image pixel size
  int  region_min[2], region_max[2];  // Render region

  Vec3 eye, coi, vup;
  Flt  fov;
  int  samples_x, samples_y;  // Sample grid (for supersampling)
  Flt  jitter;

  bool shadow, reflect, transmit;
  int  max_ray_depth;
  Flt  min_ray_value;
  Flt  ray_moveout;


  Scene() { clear(); }
  ~Scene();

  // Member Functions
  void clear();
  int addObject(const ObjectPtr& ob);
  int addLight(const LightPtr& lt);
  int addShader(const ShaderPtr& sh, SceneItemFlag flag);
  int init();
  void info(std::ostream& out) const;
  int traceRay(const Ray& r, Color& result) const;
  int traceShadowRay(const Ray& r, Color& result) const;

  const std::vector<ObjectPtr>& objects() const { return _objects; }
  const std::vector<LightPtr>& lights() const { return _lights; }
  const ObjectPtr& bound() const { return _bound; }

 private:
  std::vector<ObjectPtr> _objects;
    // Complete list of objects (including Groups but not Bounds)

  ObjectPtr _bound; // Bounding box hierarchy of objects

  std::vector<LightPtr> _lights;

  std::vector<ShaderPtr> _shaders;
    // all top-level shader objects (for initialization)
};
