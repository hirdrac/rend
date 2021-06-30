//
// Group.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Group.hh"
#include "Stats.hh"
#include <sstream>


// **** Group Class ****
// Constructor
Group::Group()
{
  ++Inventory.groups;
}

// Destructor
Group::~Group()
{
  --Inventory.groups;
}

// SceneItem Functions
int Group::add(SceneItem* i, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(i);
  if (ob) {
    _childList.addToTail(ob);
    return 0;
  } else {
    return Primitive::add(i, flag);
  }
}

std::string Group::desc() const
{
  std::ostringstream os;
  os << "<Group";
  if (!_name.empty()) { os << " \"" << _name << '\"'; }
  os << '>';
  return os.str();
}

int Group::init(Scene& s)
{
  return InitObjectList(s, _childList.head(), shader(), &_trans);
}

// Object Functions
int Group::intersect(const Ray& r, HitList& hit_list) const
{
  int hits = 0;
  for (const Object* ob = _childList.head(); ob != nullptr; ob = ob->next()) {
    hits += ob->intersect(r, hit_list);
  }
  return hits;
}
