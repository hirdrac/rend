//
// Scene.hh
// Copyright (C) 2023 Richard Bradley
//
// storage class for all scene information
//

#pragma once
#include "ObjectPtr.hh"
#include "LightPtr.hh"
#include "ShaderPtr.hh"
#include "SceneItem.hh"
#include "HitCostInfo.hh"
#include "Types.hh"
#include <vector>
#include <span>


// **** Types ****
class Color;
struct JobState;
class Ray;


class Scene
{
 public:
  // Scene color shaders
  ShaderPtr ambient;
  ShaderPtr background;

  // Default shaders
  ShaderPtr default_obj;
  ShaderPtr default_lt;

  int image_width, image_height;     // Image pixel size
  int region_min[2], region_max[2];  // Render region

  // camera settings
  Vec3 eye{INIT_NONE};      // camera location
  Vec3 coi{INIT_NONE};      // center-of-interest
  Vec3 vup{INIT_NONE};      // view-up vector
  Flt  fov;                 // field of view angle
  Flt  aperture;            // depth-of-field jitter amount when positive
  Flt  focus;               // focal distance

  // anti-aliasing
  int  sample_x, sample_y;  // sub-pixel grid size
  Flt  jitter;              // x/y jitter amount for a sub-pixel
  int  samples;             // sample count for a sub-pixel if jittering

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

  // intersection cost estimate
  HitCostInfo hitCosts;


  Scene() { clear(); }
  ~Scene();

  // Member Functions
  void clear();
  int addObject(const ObjectPtr& ob);
  int addLight(const LightPtr& lt);
  int addShader(const ShaderPtr& sh, SceneItemFlag flag);
  int init();

  [[nodiscard]] Color traceRay(JobState& js, const Ray& r) const;
  [[nodiscard]] bool castShadowRay(JobState& js, const Ray& r) const;

  [[nodiscard]] std::span<const ObjectPtr> objects() const {
    return _objects; }
  [[nodiscard]] std::span<const ObjectPtr> optObjects() const {
    return _optObjects; }
  [[nodiscard]] std::span<const LightPtr> lights() const {
    return _lights; }

  [[nodiscard]] int samplesPerPixel() const {
    const bool multiSample = IsPositive(jitter) || IsPositive(aperture);
    return (std::max(sample_x, 1) * std::max(sample_y, 1))
      * (multiSample ? std::max(samples, 1) : 1);
  }

 private:
  std::vector<ObjectPtr> _objects;
    // Complete list of objects (including Groups but not Bounds)

  std::vector<ObjectPtr> _optObjects;
    // bounding box optimized objects

  std::vector<LightPtr> _lights;

  std::vector<ShaderPtr> _shaders;
    // all top-level shader objects (for initialization)
};
