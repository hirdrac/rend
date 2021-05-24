//
// SceneItem.hh
// Copyright (C) 2021 Ricahrd Bradley
//

#pragma once
#include <string>
#include <ostream>


// **** Types ****
class Scene;
class Transform;


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
  // Destructor
  virtual ~SceneItem() = default;

  // Member Function
  virtual const char*      name() const { return nullptr; }
  virtual Transform*       trans()       { return nullptr; }
  virtual const Transform* trans() const { return nullptr; }
  virtual std::string desc(int indent) const = 0;

  virtual int setName(const std::string& val) { return -1; }
  virtual int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) { return -1; }

  virtual int init(Scene& s) { return 0; }
};


// **** Inlined Functions ****
template<class T>
void PrintList(std::ostream& out, T* list, int indent = 0)
{
  for (int i = 0; i < indent; ++i) { out << ' '; }

  if (!list) {
    out << "<EMPTY>\n";
    return;
  }

  for (; list != nullptr; list = list->next()) {
    out << list->desc(indent);
  }
}

template<class ContainerT>
void PrintList2(std::ostream& out, const ContainerT& x, int indent = 0)
{
  //while (--indent >= 0) { out << ' '; }
  for (int i = 0; i < indent; ++i) { out << ' '; }

  if (x.empty()) {
    out << "<EMPTY>\n";
    return;
  }

  for (SceneItem* item : x) { out << item->desc(indent); }
}
