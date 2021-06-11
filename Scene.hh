//
// Scene.hh
// Copyright (C) 2021 Richard Bradley
//
// Definition of scene class
//

#pragma once
#include "Object.hh"
#include "Light.hh"
#include "Shader.hh"
#include "Types.hh"
#include "SList.hh"
#include <vector>


// **** Types ****
class SceneDesc;
class SceneItem;
class Color;
class Ray;


// Scene class
//  storage class for all scene information
class Scene
{
 public:
  SList<Object> object_list; // Complete list of objects (including Groups)
  SList<Object> bound_list;  // Bounding box heiarchy of objects
  std::vector<Light*> lights;

  // Scene color shaders
  Shader* ambient;
  Shader* background;
  Shader* air;

  // Default shaders
  Shader* default_obj;
  Shader* default_lt;

  int  image_width, image_height;     // Image pixel size
  int  region_min[2], region_max[2];  // Render region

  Vec3 eye, coi, vup;
  Flt  fov, index;
  int  samples_x, samples_y;  // Sample grid (for supersampling)

  bool shadow, reflect, transmit;
  int  max_ray_depth;
  Flt  min_ray_value;

  // Constructor
  Scene() { clear(); }
  // Destructor
  ~Scene();

  // Member Functions
  void clear();
  int generate(SceneDesc& sd);
  int add(SceneItem* i, SceneItemFlag flag);
  int init();
  void info(std::ostream& out) const;
  int traceRay(const Ray& r, Color& result) const;
  int traceShadowRay(const Ray& r, Color& result) const;

 private:
  std::vector<Shader*> _shaders;
};
