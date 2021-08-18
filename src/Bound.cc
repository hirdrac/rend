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
#include "HitCostInfo.hh"
#include <sstream>
#include <vector>


// **** Bound Class ****
std::string Bound::desc() const
{
  std::ostringstream os;
  os << "<Bound " << objects.size() << '>';
  return os.str();
}

int Bound::intersect(const Ray& r, HitList& hit_list) const
{
  ++r.stats->bound.tried;
  Flt near_hit = -VERY_LARGE, far_hit = VERY_LARGE;

  for (unsigned int i = 0; i < 3; ++i) {
    Flt h1 = (box.pmin[i] - r.base[i]) / r.dir[i];
    Flt h2 = (box.pmax[i] - r.base[i]) / r.dir[i];
    // NOTE: can generate false hits when r.dir[i] == 0 since
    //   r.base[i] isn't checked

    if (h1 > h2) { std::swap(h1, h2); }
    if (h1 > near_hit) { near_hit = h1; }
    if (h2 < far_hit) { far_hit = h2; }
  }

  // alternate version
  //const Vec3 h1 = (box.pmin - r.base) / r.dir;
  //const Vec3 h2 = (box.pmax - r.base) / r.dir;
  //for (unsigned int i = 0; i < 3; ++i) {
  //  const Flt h_min = std::min(h1[i], h2[i]);
  //  const Flt h_max = std::max(h1[i], h2[i]);
  //  if (h_min > near_hit) { near_hit = h_min; }
  //  if (h_max < far_hit) { far_hit = h_max; }
  //}

  if (near_hit > far_hit || far_hit < r.min_length) {
    return 0;  // miss
  }

  ++r.stats->bound.hit;

  // Intersect all contained objects
  int hits = 0;
  for (auto& ob : objects) {
    hits += ob->intersect(r, hit_list);
  }

  return hits;
}


// **** Prototypes ****
struct OptNode;
static Flt treeCost(const OptNode* node_list, Flt bound_weight);


// **** OptNode struct ****
struct OptNode
{
  OptNode* next = nullptr;
  OptNode* child = nullptr;
  ObjectPtr object; // null if node is bound (has children)
  BBox box;
  Flt objCost;

  OptNode() = default;
  OptNode(const ObjectPtr& ob) : object(ob) {
    ob->bound(box);
    objCost = ob->hitCost();
  }

  // Member Functions
  Flt cost(Flt weight) const
  {
    if (object) {
      return weight * objCost;
    } else {
      // node is bound
      return (weight * CostTable.bound) + treeCost(child, box.weight());
    }
  }
};


// **** Functions ****
static void killTree(OptNode* node_list)
{
  while (node_list) {
    OptNode* next = node_list->next;
    if (node_list->child) {
      killTree(node_list->child);
    }

    delete node_list;
    node_list = next;
  }
}

static Flt treeCost(const OptNode* node_list, Flt bound_weight)
{
  Flt total = 0;
  for (; node_list != nullptr; node_list = node_list->next) {
    total += node_list->cost(bound_weight);
  }

  return total;
}

static Flt mergeCost(const OptNode* n1, const OptNode* n2, Flt weight)
{
  const Flt add_c = n1->cost(weight) + n2->cost(weight);

  const Flt w = BBox(n1->box, n2->box).weight();
  const Flt m_cost1 = n1->object ? n1->cost(w) : treeCost(n1->child, w);
  const Flt m_cost2 = n2->object ? n2->cost(w) : treeCost(n2->child, w);
  const Flt merge_c = (weight * CostTable.bound) + m_cost1 + m_cost2;

  // positive value indicates improvement if merged
  return add_c - merge_c;
}

static OptNode* mergeOptNodes(OptNode* node1, OptNode* node2)
{
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
  b->child = n1;
  b->box   = BBox(node1->box, node2->box);
  while (n1->next) { n1 = n1->next; }
  n1->next = n2;

  return b;
}

static OptNode* makeOptNodeList(const std::vector<ObjectPtr>& o_list)
{
  OptNode* node_list = nullptr;
  OptNode* tail = nullptr;
  for (auto& ob : o_list) {
    OptNode* list = nullptr;
    if (const Group* gPtr = dynamic_cast<const Group*>(ob.get()); gPtr) {
      list = makeOptNodeList(gPtr->children());
      if (!list) { continue; }
    } else {
      list = new OptNode{ob};
    }

    if (tail) { tail->next = list; } else { node_list = tail = list; }
    while (tail->next) { tail = tail->next; }
  }

  return node_list;
}

static int optimizeOptNodeList(OptNode*& node_list, Flt weight, int depth)
{
  if (!node_list) {
    LOG_ERROR("Optimizing empty list");
    return 0;
  }

  // create array to index nodes
  std::vector<OptNode*> node_array;
  for (OptNode* ptr = node_list; ptr != nullptr; ) {
    OptNode* n = ptr;
    ptr = ptr->next;
    n->next = nullptr;

    const Flt cost1 = n->cost(weight);
    const Flt cost2 = (weight * CostTable.bound) + n->cost(n->box.weight());
    if (cost1 > cost2) {
      // Put object into bound (alone)
      OptNode* b = new OptNode;
      b->child = n;
      b->box   = n->box;
      node_array.push_back(b);
    } else {
      node_array.push_back(n);
    }
  }

  auto node_count = node_array.size();
  if (node_count > 1) {
    // Group objects together in bounding boxes
    for (;;) {
      Flt best = 0;
      unsigned int best_i = 0, best_j = 0;

      for (unsigned int i = 0; i < (node_count-1); ++i) {
        OptNode* n1 = node_array[i];
        if (!n1) { continue; }

        for (unsigned int j = i+1; j < node_count; ++j) {
          OptNode* n2 = node_array[j];
          if (!n2) { continue; }

          Flt save = mergeCost(n1, n2, weight);
          if (save >= best) {
            best = save;
            best_i = i;
            best_j = j;
          }
        }
      }

      if (best <= 0) { break; }

      node_array[best_i] =
        mergeOptNodes(node_array[best_i], node_array[best_j]);
      node_array[best_j] = nullptr;
    }
  }

  // Reconstruct linked list
  node_list = nullptr;
  OptNode* prev = nullptr;
  for (auto n : node_array) {
    if (n) {
      if (prev) { prev->next = n; }
      else { node_list = n; }
      prev = n;
    }
  }

  // Optimize inside bounds (recursively)
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (!n->object) {
      // bound node
      int error = optimizeOptNodeList(n->child, n->box.weight(), depth + 1);
      if (error) { return error; }
    }
  }

  return 0;
}

static int convertNodeList(
  OptNode* node_list, std::vector<ObjectPtr>& bound_list, BBox& bound_box)
{
  if (!node_list) { return -1; }

  int count = 0;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->object) {
      // object node
      bound_list.push_back(n->object);
      bound_box.fit(n->box);
    } else {
      // bound node
      auto b = std::make_shared<Bound>();
      int no = convertNodeList(n->child, b->objects, b->box);
      if (no >= 0) {
        bound_list.push_back(b);
	bound_box.fit(b->box);
        count += 1 + no;
      }
    }
  }

  return count;
}

int MakeBoundList(const Vec3& eye, const std::vector<ObjectPtr>& o_list,
                  std::vector<ObjectPtr>& bound_list)
{
  OptNode* node_list = makeOptNodeList(o_list);
  if (!node_list) {
    return 0;
  }

  BBox box{eye};
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    box.fit(n->box);
  }

  println("Old tree cost: ", treeCost(node_list, box.weight()));
  if (optimizeOptNodeList(node_list, box.weight(), 0)) {
    killTree(node_list);
    return 0;
  }

  println("New tree cost: ", treeCost(node_list, box.weight()));

  BBox bound_box;
  bound_list.clear();
  int count = convertNodeList(node_list, bound_list, bound_box);
  killTree(node_list);
  return count;
}
