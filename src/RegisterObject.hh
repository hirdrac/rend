//
// RegisterObject.hh
// Copyright (C) 2022 Richard Bradley
//

#pragma once
#include "Keywords.hh"
#include "Parser.hh"
#include "ObjectPtr.hh"
#include "Scene.hh"


template<class ObjectType>
int ObjectItemFn(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                 SceneItemFlag) {
  auto ob = makeObject<ObjectType>();
  const int error = p ? p->addObject(ob) : s.addObject(ob);
  return error ? error : sp.processList(s, ob.get(), n);
}

#define REGISTER_OBJECT_CLASS(type,keyword)\
  static bool _object_keyword_##type = AddItemFn(keyword,&ObjectItemFn<type>)
