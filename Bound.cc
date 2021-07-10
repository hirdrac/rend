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
#include "Print.hh"
#include "PrintList.hh"
#include <sstream>
#include <vector>


// **** Prototypes ****
struct OptNode;
Flt TreeWeight(const OptNode* node_list, Flt weight);


// **** OptNode struct ****
struct OptNode
{
  OptNode* next = nullptr;
  OptNode* child = nullptr;
  ObjectPtr object; // null if node is bound (has children)
  BBox box;

  OptNode() = default;
  OptNode(const ObjectPtr& ob) : object(ob) { ob->bound(box); }

  // Member Functions
  Flt cost(Flt weight) const
  {
    if (object) {
      return weight * object->hitCost();
    } else {
      // node is bound
      return (weight * CostTable.bound) + TreeWeight(child, box.weight());
    }
  }
};


// **** Bound Class ****
Bound::Bound()
{
  ++Inventory.bounds;
}

Bound::~Bound()
{
  --Inventory.bounds;
}

// Member Functions
std::string Bound::desc() const
{
  std::ostringstream os;
  os << "<Bound " << objects.size() << '>';
  return os.str();
}

int Bound::intersect(const Ray& r, HitList& hit_list) const
{
  if (!always_hit) {
    ++r.stats->bound_tried;
    Flt near_hit = -VERY_LARGE, far_hit = VERY_LARGE;

    // X
    if (!IsZero(r.dir.x)) {
      Flt h1 = (box.pmin.x - r.base.x) / r.dir.x;
      Flt h2 = (box.pmax.x - r.base.x) / r.dir.x;

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
      Flt h1 = (box.pmin.y - r.base.y) / r.dir.y;
      Flt h2 = (box.pmax.y - r.base.y) / r.dir.y;

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
      Flt h1 = (box.pmin.z - r.base.z) / r.dir.z;
      Flt h2 = (box.pmax.z - r.base.z) / r.dir.z;

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

    ++r.stats->bound_hit;
  }

  // Intersect all contained objects
  int hits = 0;
  for (auto& ob : objects) {
    hits += ob->intersect(r, hit_list);
  }

  return hits;
}


// **** Functions ****
unsigned int CountList(const OptNode* node_list)
{
  unsigned int count = 0;
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

void PrintBoundList(const OptNode* node_list, int indent = 0)
{
  if (!node_list) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println("*EMPTY*");
    return;
  }

  while (node_list) {
    for (int i = 0; i < indent; ++i) { print(' '); }

    auto& ob = node_list->object;
    if (ob) {
      println(ob->desc());
      if (!(ob->children().empty())) {
        PrintList(ob->children(), indent + 2);
      }
    } else {
      println("<Bound>");
      PrintBoundList(node_list->child, indent + 2);
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

  Flt cost1 = node1->object ? node1->cost(w) : TreeWeight(node1->child, w);
  Flt cost2 = node2->object ? node2->cost(w) : TreeWeight(node2->child, w);
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
  OptNode* b = new OptNode();
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

OptNode* MakeOptNodeList(const std::vector<ObjectPtr>& o_list)
{
  OptNode* node_list = nullptr;

  for (auto& ob : o_list) {
    const Group* gPtr = dynamic_cast<const Group*>(ob.get());
    if (gPtr) {
      // Add contents of group
      OptNode* sub_list = MakeOptNodeList(gPtr->children());
      if (sub_list) {
        OptNode* tmp = sub_list;
        while (tmp->next) { tmp = tmp->next; }
        tmp->next = node_list;
        node_list = sub_list;
      }
    } else if (ob->isVisible()) {
      // Create new node for object
      OptNode* n = new OptNode(ob);
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

  unsigned int node_count = CountList(node_list);
  std::vector<OptNode*> node_array(node_count);

  // create array to index nodes
  OptNode* n = node_list;
  for (unsigned int i = 0; i < node_count; ++i) {
    node_array[i] = n;
    n = n->next;
    node_array[i]->next = nullptr;
  }

  // bound any expensive objects
  for (unsigned int i = 0; i < node_count; ++i) {
    n = node_array[i];
    Flt cost1 = n->cost(weight);
    Flt cost2 = (weight * CostTable.bound) + n->cost(n->box.weight());
    if (cost1 > cost2) {
      // Put object into bound (alone)
      OptNode* b = new OptNode();
      b->child = n;
      b->box   = n->box;

      node_array[i] = b;
    }
  }

  // Group objects together in bounding boxes
  if (node_count > 1) {
    unsigned int best_i = 0, best_j = 0;
    Flt save;
    do {
      Flt best = 0;
      save = 0;
      for (unsigned int i = 0; i < (node_count-1); ++i) {
        if (!node_array[i]) { continue;	}

	for (unsigned int j = i+1; j < node_count; ++j) {
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
  for (unsigned int i = 1; i < node_count; ++i) {
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

std::shared_ptr<Bound> ConvertNodeList(OptNode* node_list)
{
  if (!node_list) {
    return std::shared_ptr<Bound>();
  }

  auto b = std::make_shared<Bound>();

  // set up bound
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->object) {
      b->objects.push_back(n->object);
      b->box.fit(n->box);
    } else {
      auto child_b = ConvertNodeList(n->child);
      if (child_b) {
        b->objects.push_back(child_b);
	b->box.fit(child_b->box);
      }
    }
  }

  return b;
}

ObjectPtr MakeBoundList(const std::vector<ObjectPtr>& o_list)
{
  OptNode* node_list = MakeOptNodeList(o_list);
  if (!node_list) {
    return nullptr;
  }

  BBox box;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    box.fit(n->box);
  }

  println("Old tree weight: ", TreeWeight(node_list, box.weight()));
  if (OptimizeOptNodeList(node_list, box.weight())) {
    return nullptr;
  }

  println("New tree weight: ", TreeWeight(node_list, box.weight()));
  PrintBoundList(node_list);

  auto bound = ConvertNodeList(node_list);
  bound->always_hit = true;
  KillTree(node_list);
  return bound;
}
