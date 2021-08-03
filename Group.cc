//
// Group.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Group.hh"
#include "Stats.hh"
#include <sstream>


// **** Group Class ****
Group::Group()
{
  ++Inventory.groups;
}

Group::~Group()
{
  --Inventory.groups;
}

// SceneItem Functions
int Group::addObject(const ObjectPtr& ob)
{
  if (!ob) { return -1; }

  _children.push_back(ob);
  return 0;
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
  for (auto& ob : _children) {
    if (InitObject(s, *ob, _shader, &_trans)) { return -1; }
  }
  return 0;
}

// Object Functions
int Group::intersect(const Ray& r, bool csg, HitList& hit_list) const
{
  int hits = 0;
  for (auto& ob : _children) { hits += ob->intersect(r, csg, hit_list); }
  return hits;
}
