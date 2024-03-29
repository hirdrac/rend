//
// SceneItem.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "LightPtr.hh"
#include "ShaderPtr.hh"
#include "ObjectPtr.hh"
#include "Types.hh"
#include <string>


// **** Types ****
class Scene;
class Transform;

enum SceneItemFlag {
  FLAG_NONE,
  // scene shader types
  FLAG_AMBIENT, FLAG_BACKGROUND, FLAG_DEFAULT_LT, FLAG_DEFAULT_OBJ,
  FLAG_INIT_ONLY,
  // phong shader types
  FLAG_DIFFUSE, FLAG_SPECULAR, FLAG_TRANSMIT,
  // pattern shaders
  FLAG_BORDER,
};


// SceneItem class definition
//  provides interface for parser to modify scene elements
//
class SceneItem
{
 public:
  virtual ~SceneItem() = default;

  // Member Function
  virtual std::string desc() const = 0;
  virtual Transform* trans() { return nullptr; }

  virtual int setBorderwidth(Flt v) { return -1; }
  virtual int setCost(Flt v) { return -1; }
  virtual int setOffset(Flt v) { return -1; }
  virtual int setRadius(Flt v) { return -1; }
  virtual int setSamples(int v) { return -1; }
  virtual int setSectors(int v) { return -1; }
  virtual int setSides(int v) { return -1; }
  virtual int setSpin(Flt v) { return -1; }
  virtual int setValue(Flt v) { return -1; }

  virtual int addObject(const ObjectPtr& ob) { return -1; }
  virtual int addLight(const LightPtr& lt) { return -1; }
  virtual int addShader(const ShaderPtr& sh, SceneItemFlag flag) { return -1; }
};
