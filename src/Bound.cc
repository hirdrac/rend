//
// Bound.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Bound.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Stats.hh"
#include "Group.hh"
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

  Flt objHitCost;   // cache Object::hitCost()
  Flt currentCost;  // cache OptNode::cost()

  OptNode() = default;
  OptNode(const ObjectPtr& ob)
    : object{ob}, box{ob->bound()}, objHitCost{ob->hitCost()} { }

  // Member Functions
  Flt cost(Flt weight) const
  {
    if (object) {
      return weight * objHitCost;
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

static Flt calcMergeCost(const OptNode* n1, const OptNode* n2, Flt weight)
{
  const Flt w = BBox(n1->box, n2->box).weight();
  const Flt m_cost1 = n1->object ? n1->cost(w) : treeCost(n1->child, w);
  const Flt m_cost2 = n2->object ? n2->cost(w) : treeCost(n2->child, w);
  return (weight * CostTable.bound) + m_cost1 + m_cost2;
}

static OptNode* mergeOptNodes(OptNode* node1, OptNode* node2)
{
  // Create new bounding box
  OptNode* b = new OptNode;
  b->box = BBox(node1->box, node2->box);

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

  b->child = n1;
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

static int optimizeOptNodeList(OptNode*& node_list, Flt weight)
{
  // create array to index nodes
  std::vector<OptNode*> node_array;
  for (OptNode* ptr = node_list; ptr != nullptr; ) {
    OptNode* n = ptr;
    ptr = ptr->next;
    n->next = nullptr;

    const Flt cost1 = n->cost(weight);
    n->currentCost = cost1;
    const Flt cost2 = (weight * CostTable.bound) + n->cost(n->box.weight());
    if (cost1 > cost2) {
      // Put object into bound (alone)
      OptNode* b = new OptNode;
      b->child = n;
      b->box   = n->box;
      b->currentCost = cost2;
      n = b;
    }
    node_array.push_back(n);
  }

  auto node_count = node_array.size();
  if (node_count > 1) {
    // Group objects together in bounding boxes
    for (;;) {
      Flt best = 0;
      Flt bestMergeCost = 0;
      unsigned int best_i = 0, best_j = 0;

      for (unsigned int i = 0; i < (node_count-1); ++i) {
        OptNode* n1 = node_array[i];
        if (!n1) { continue; }

        for (unsigned int j = i+1; j < node_count; ++j) {
          OptNode* n2 = node_array[j];
          if (!n2) { continue; }

          const Flt baseCost = n1->currentCost + n2->currentCost;
          const Flt mergeCost = calcMergeCost(n1, n2, weight);
          const Flt costImprove = baseCost - mergeCost;
          if (costImprove > best) {
            best = costImprove;
            bestMergeCost = mergeCost;
            best_i = i;
            best_j = j;
          }
        }
      }

      if (best <= 0) { break; }

      OptNode* n = mergeOptNodes(node_array[best_i], node_array[best_j]);
      n->currentCost = bestMergeCost;
      node_array[best_i] = n;
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
    if (!n->object && n->child->next) {
      // bound node with 2 or more children
      int error = optimizeOptNodeList(n->child, n->box.weight());
      if (error) { return error; }
    }
  }

  return 0;
}

static int convertNodeSubList(
  OptNode* node_list, std::vector<ObjectPtr>& bound_list, BBox& bound_box)
{
  int count = 0;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->object) {
      // object node
      bound_list.push_back(n->object);
      bound_box.fit(n->box);
    } else {
      // bound node
      auto b = std::make_shared<Bound>();
      count += 1 + convertNodeSubList(n->child, b->objects, b->box);
      bound_list.push_back(b);
      bound_box.fit(b->box);
    }
  }

  return count;
}

static int convertNodeList(
  OptNode* node_list, std::vector<ObjectPtr>& bound_list)
{
  if (!node_list) { return -1; }

  int count = 0;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->object) {
      // object node
      bound_list.push_back(n->object);
    } else {
      // bound node
      auto b = std::make_shared<Bound>();
      count += 1 + convertNodeSubList(n->child, b->objects, b->box);
      bound_list.push_back(b);
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

  const Flt scene_weight = box.weight();
  println("Old tree cost: ", treeCost(node_list, scene_weight));

  if (optimizeOptNodeList(node_list, scene_weight)) {
    killTree(node_list);
    return 0;
  }

  println("New tree cost: ", treeCost(node_list, scene_weight));

  bound_list.clear();
  int count = convertNodeList(node_list, bound_list);
  killTree(node_list);
  return count;
}
