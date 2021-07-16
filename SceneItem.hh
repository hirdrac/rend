//
// SceneItem.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "LightPtr.hh"
#include "ShaderPtr.hh"
#include "ObjectPtr.hh"
#include <string>


// **** Types ****
class Scene;
class Transform;

enum SceneItemFlag {
  NO_FLAG, AIR, AMBIENT, BACKGROUND, DEFAULT_LT, DEFAULT_OBJ,
  DIFFUSE, SPECULAR, TRANSMIT
};


// SceneItem class definition
//  provides interface for parser to modify scene elements
//
class SceneItem
{
 public:
  virtual ~SceneItem() = default;

  // Member Function
  virtual Transform*       trans()       { return nullptr; }
  virtual const Transform* trans() const { return nullptr; }
  virtual std::string desc() const = 0;
  virtual int setName(const std::string& val) { return -1; }

  virtual int addObject(const ObjectPtr& ob) { return -1; }
  virtual int addLight(const LightPtr& lt) { return -1; }
  virtual int addShader(const ShaderPtr& sh, SceneItemFlag flag) { return -1; }
};
