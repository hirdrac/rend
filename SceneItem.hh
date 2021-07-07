//
// SceneItem.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <string>


// **** Types ****
class Scene;
class Transform;
class Object;
class Light;
class Shader;


enum SceneItemFlag {
  NO_FLAG, AIR, AMBIENT, BACKGROUND, BUMPMAP, DEFAULT_LT, DEFAULT_OBJ,
  DIFFUSE, INTERIOR, SPECULAR, SURFACE, TRANSMIT
};


// SceneItem class definition
//  base class for scene elements - provides a standard set of
//  functions for printing, list management, copying, and transforming
//  elements
class SceneItem
{
 public:
  virtual ~SceneItem() = default;

  // Member Function
  virtual Transform*       trans()       { return nullptr; }
  virtual const Transform* trans() const { return nullptr; }
  virtual std::string desc() const = 0;
  virtual int setName(const std::string& val) { return -1; }

  virtual int addObject(Object* ob) { return -1; }
  virtual int addLight(Light* lt) { return -1; }
  virtual int addShader(Shader* sh, SceneItemFlag flag) { return -1; }

  virtual int init(Scene& s) { return 0; }
};
