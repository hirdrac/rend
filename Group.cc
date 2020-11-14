//
// Group.cc - revision 1 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//

#include "Group.hh"
#include "Stats.hh"


/**** Group Class ****/
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
    child_list.addToTail(ob);
    return 0;
  } else {
    return Primitive::add(i, flag);
  }
}

void Group::print(std::ostream& out, int indent) const
{
  out << "<Group";
  if (!_name.empty()) {
    out << " \"" << _name << '\"';
  }

  out << '>';

  if (!child_list.empty()) {
    out << '\n';
    PrintList(out, child_list.head(), indent + 2);
    out << '\n';
  }
}

int Group::init(Scene& s)
{
  return InitObjectList(s, child_list.head(), shader(), &_trans);
}

// Object Functions
int Group::intersect(const Ray& r, HitList& hit_list) const
{
  int hits = 0;
  for (const Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    hits += ob->intersect(r, hit_list);
  }
  return hits;
}
