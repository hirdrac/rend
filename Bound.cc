//
// Bound.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of bound module
//

#include "Bound.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Stats.hh"
#include "Group.hh"
#include "Logger.hh"
#include <iostream>
#include <sstream>
#include <vector>


// **** Prototypes ****
class OptNode;
Flt TreeWeight(const OptNode* node_list, Flt weight);


// **** OptNode class ****
class OptNode
{
 public:
  OptNode* next = nullptr;
  OptNode* child = nullptr;
  const Object* object; // null if node is bound (has children)
  BBox box;

  // Constructor
  OptNode(const Object* ob = nullptr) : object(ob)
  {
    if (ob) { ob->bound(box); }
  }

  // Member Functions
  Flt cost(Flt weight) const
  {
    if (object) {
      return (weight * object->hitCost());
    } else {
      // node is bound
      return (weight * CostTable.bound) + TreeWeight(child, box.weight());
    }
  }

 private:
  // prevent copy/assign
  OptNode(const OptNode&) = delete;
  OptNode& operator=(const OptNode&) = delete;
};


// **** Bound Class ****
// Constructor
Bound::Bound()
{
  ++Inventory.bounds;
}

// Destructor
Bound::~Bound()
{
  --Inventory.bounds;
}

// Member Functions
std::string Bound::desc(int) const
{
  std::ostringstream os;
  os << "<Bound " << object_list.size() << ">\n";
  return os.str();
}

int Bound::intersect(const Ray& r, HitList& hit_list) const
{
  if (!always_hit) {
    ++Stats.bound_tried;
    Flt h1, h2, near_hit = -VERY_LARGE, far_hit = VERY_LARGE;

    // X
    if (!IsZero(r.dir.x)) {
      h1 = (box.pmin.x - r.base.x) / r.dir.x;
      h2 = (box.pmax.x - r.base.x) / r.dir.x;

      if (h1 > h2)  {
	near_hit = h2; far_hit = h1;
      } else  {
	near_hit = h1; far_hit = h2;
      }
    } else if ((r.base.x < box.pmin.x) || (r.base.x > box.pmax.x)) {
      return 0;  // Miss
    }

    // Y
    if (!IsZero(r.dir.y)) {
      h1 = (box.pmin.y - r.base.y) / r.dir.y;
      h2 = (box.pmax.y - r.base.y) / r.dir.y;

      if (h1 > h2)  {
	if (h2 > near_hit) { near_hit = h2; }
	if (h1 < far_hit)  { far_hit  = h1; }
      } else  {
	if (h1 > near_hit) { near_hit = h1; }
	if (h2 < far_hit)  { far_hit  = h2; }
      }

      if (near_hit > far_hit) {
	return 0;  // Miss
      }
    } else if ((r.base.y < box.pmin.y) || (r.base.y > box.pmax.y)) {
      return 0;  // Miss
    }

    // Z
    if (!IsZero(r.dir.z)) {
      h1 = (box.pmin.z - r.base.z) / r.dir.z;
      h2 = (box.pmax.z - r.base.z) / r.dir.z;

      if (h1 > h2)  {
	if (h2 > near_hit) { near_hit = h2; }
	if (h1 < far_hit)  { far_hit  = h1; }
      } else  {
	if (h1 > near_hit) { near_hit = h1; }
	if (h2 < far_hit)  { far_hit  = h2; }
      }

      if (near_hit > far_hit) {
	return 0;  // Miss
      }
    } else if ((r.base.z < box.pmin.z) || (r.base.z > box.pmax.z)) {
      return 0;  // Miss
    }

    if (far_hit < 0) {
      return 0;  // cube completely behind ray origin
    }

    ++Stats.bound_hit;
  }

  // bounding box hit - intersect child bounds
  int hits = 0;
  for (Object* ob = child_list.head(); ob != nullptr; ob = ob->next()) {
    hits += ob->intersect(r, hit_list);
  }

  // Intersect all child objects
  for (const Object* ob : object_list) {
    hits += ob->intersect(r, hit_list);
  }

  return hits;
}


// **** Functions ****
int CountList(const OptNode* node_list)
{
  int count = 0;
  for (; node_list != nullptr; node_list = node_list->next) { ++count; }
  return count;
}

void KillTree(OptNode* node_list)
{
  while (node_list) {
    OptNode* next = node_list->next;
    if (node_list->child) {
      KillTree(node_list->child);
    }

    delete node_list;
    node_list = next;
  }
}

void PrintBoundList(const OptNode* node_list, std::ostream& os,
		    int indent = 0)
{
  if (!node_list) {
    for (int i = 0; i < indent; ++i) { os << ' '; }
    os << "EMPTY\n";
    return;
  }

  while (node_list) {
    for (int i = 0; i < indent; ++i) { os << ' '; }

    if (node_list->object) {
      os << node_list->object->desc(indent + 2) << '\n';
    } else {
      os << "<Bound>\n";
      PrintBoundList(node_list->child, os, indent + 2);
    }

    node_list = node_list->next;
  }
}

Flt TreeWeight(const OptNode* node_list, Flt weight)
{
  Flt total = 0;
  for (; node_list != nullptr; node_list = node_list->next) {
    total += node_list->cost(weight);
  }

  return total;
}

Flt MergeCost(const OptNode* node1, const OptNode* node2, Flt weight)
{
  BBox box(node1->box);
  box.fit(node2->box);
  Flt w = box.weight();

  Flt cost1 = node1->object ? node1->cost(w) : node1->child->cost(w);
  Flt cost2 = node2->object ? node2->cost(w) : node2->child->cost(w);
  return (weight * CostTable.bound) + cost1 + cost2;
}

OptNode* MergeOptNodes(OptNode* node1, OptNode* node2)
{
  BBox box = node1->box;
  box.fit(node2->box);

  OptNode* n1 = node1;
  if (!node1->object) {
    // remove bound node
    n1 = node1->child;
    delete node1;
  }

  OptNode* n2 = node2;
  if (!node2->object) {
    // remove bound node
    n2 = node2->child;
    delete node2;
  }

  // Create new bounding box
  OptNode* b = new OptNode;
  b->box   = box;
  b->child = n1;
  while (n2) {
    OptNode* n = n2->next;
    n2->next = b->child;
    b->child = n2;
    n2 = n;
  }

  return b;
}

OptNode* MakeOptNodeList(const Object* o_list)
{
  OptNode* node_list = nullptr;

  for (const Object* o = o_list; o != nullptr; o = o->next()) {
    if (dynamic_cast<const Group*>(o)) {
      // Add contents of group
      OptNode* sub_list = MakeOptNodeList(o->childList());
      if (sub_list) {
        OptNode* tmp = sub_list;
        while (tmp->next) { tmp = tmp->next; }
        tmp->next = node_list;
        node_list = sub_list;
      }
    } else if (o->isVisible()) {
      // Create new node for object
      OptNode* n = new OptNode(o);
      n->next = node_list;
      node_list = n;
    }
  }

  return node_list;
}

int OptimizeOptNodeList(OptNode*& node_list, Flt weight, int depth = 0)
{
  if (!node_list) {
    LOG_ERROR("Optimizing empty list");
    return 0;
  }

  int node_count = CountList(node_list);
  std::vector<OptNode*> node_array(node_count);

  // create array to index nodes
  OptNode* n = node_list;
  for (int i = 0; i < node_count; ++i) {
    node_array[i] = n;
    n = n->next;
    node_array[i]->next = nullptr;
  }

  // bound any expensive objects
  for (int i = 0; i < node_count; ++i) {
    n = node_array[i];
    Flt cost1 = n->cost(weight);
    Flt cost2 = (weight * CostTable.bound) + n->cost(n->box.weight());
    if (cost1 > cost2) {
      // Put object into bound (alone)
      OptNode* b = new OptNode;
      b->child = n;
      b->box   = n->box;

      node_array[i] = b;
    }
  }

  // Group objects together in bounding boxes
  if (node_count > 1) {
    int best_i = 0, best_j = 0;
    Flt save;
    do {
      Flt best = 0;
      save = 0;
      for (int i = 0; i < (node_count-1); ++i) {
        if (!node_array[i]) { continue;	}

	for (int j = i+1; j < node_count; ++j) {
	  if (!node_array[j]) { continue; }

	  Flt add = node_array[i]->cost(weight) + node_array[j]->cost(weight);
	  Flt merge = MergeCost(node_array[i], node_array[j], weight);
	  save = add - merge;
	  if (save > best) {
	    best = save;
	    best_i = i;
	    best_j = j;
	  }
	}
      }

      if (best > 0) {
        node_array[best_i] =
	  MergeOptNodes(node_array[best_i], node_array[best_j]);
        node_array[best_j] = nullptr;
      }
    } while (save > 0);
  }

  // Reconstruct linked list
  node_list = node_array[0];
  n = node_list;
  for (int i = 1; i < node_count; ++i) {
    if (node_array[i]) {
      n->next = node_array[i];
      n = n->next;
    }
  }

  n->next = nullptr;

  // Optimize inside bounds (recursively)
  for (n = node_list; n != nullptr; n = n->next) {
    if (!n->object) {
      // bound node
      int error = OptimizeOptNodeList(n->child, n->box.weight(), depth + 1);
      if (error) { return error; }
    }
  }

  return 0;
}

Bound* ConvertNodeList(OptNode* node_list)
{
  if (!node_list) {
    return nullptr;
  }

  Bound* b = new Bound;

  // set up bound
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->object) {
      b->object_list.push_back(n->object);
      b->box.fit(n->box);
    } else {
      Bound* child_b = ConvertNodeList(n->child);
      if (child_b) {
	b->child_list.addToTail(child_b);
	b->box.fit(child_b->box);
      }
    }
  }

  return b;
}

Bound* MakeBoundList(const Object* o_list)
{
  OptNode* node_list = MakeOptNodeList(o_list);
  if (!node_list) {
    return nullptr;
  }

  BBox box;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    box.fit(n->box);
  }

  std::cout << "Old tree weight: "
	    << TreeWeight(node_list, box.weight()) << '\n';
  if (OptimizeOptNodeList(node_list, box.weight())) {
    return nullptr;
  }

  std::cout << "New tree weight: "
	    << TreeWeight(node_list, box.weight()) << '\n';
  PrintBoundList(node_list, std::cout);

  Bound* bound_list = ConvertNodeList(node_list);
  bound_list->always_hit = true;
  KillTree(node_list);
  return bound_list;
}
