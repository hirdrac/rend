//
// Bound.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Bound.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include "Stats.hh"
#include "Group.hh"
#include "CSG.hh"
#include "Print.hh"
#include "Scene.hh"
#include <sstream>
#include <vector>


// **** Bound Class ****
std::string Bound::desc() const
{
  std::ostringstream os;
  os << "<Bound " << objects.size() << '>';
  return os.str();
}

int Bound::init(Scene& s, const Transform* tr)
{
  for (auto& ob : objects) {
    if (InitObject(s, *ob, _shader, tr)) { return -1; }
  }

  return 0;
}

int Bound::intersect(const Ray& r, HitList& hl) const
{
  ++hl.stats().bound.tried;
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

  if (near_hit > far_hit
      || far_hit < r.min_length || near_hit >= r.max_length) {
    return 0;  // miss
  }

  ++hl.stats().bound.hit;

  // Intersect all contained objects
  int hits = 0;
  for (auto& ob : objects) {
    hits += ob->intersect(r, hl);
  }

  return hits;
}


// **** Prototypes ****
struct OptNode;
static Flt treeCost(const OptNode* node_list, Flt bound_weight);


// **** OptNode struct ****
enum OptNodeType { NODE_BOUND, NODE_UNION, NODE_OBJECT };

struct OptNode
{
  OptNode* next = nullptr;
  OptNode* child = nullptr;
  ObjectPtr object; // null if node is bound (has children)
  BBox box;

  Flt objHitCost;   // cache Object::hitCost()
  Flt currentCost;  // cache OptNode::cost()
  OptNodeType type;

  // bound constructor
  OptNode(Flt cost) : objHitCost{cost}, type{NODE_BOUND} { }

  // object/union constructor
  OptNode(OptNodeType t, const ObjectPtr& ob, Flt cost)
    : object{ob}, box{ob->bound(nullptr)}, objHitCost{cost}, type{t} { }

  // Member Functions
  Flt cost(Flt weight) const
  {
    Flt c = weight * objHitCost;
    if (child) { c += treeCost(child, box.weight()); }
    return c;
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

static Flt calcMergeCost(
  const Scene& s, const OptNode* n1, const OptNode* n2, Flt weight)
{
  const Flt w = BBox(n1->box, n2->box).weight();
  const Flt m_cost1 =
    (n1->type == NODE_BOUND) ? treeCost(n1->child, w) : n1->cost(w);
  const Flt m_cost2 =
    (n2->type == NODE_BOUND) ? treeCost(n2->child, w) : n2->cost(w);
  return (weight * s.hitCosts.bound) + m_cost1 + m_cost2;
}

static OptNode* mergeOptNodes(const Scene& s, OptNode* node1, OptNode* node2)
{
  // Create new bounding box
  OptNode* b = new OptNode(s.hitCosts.bound);
  b->box = BBox(node1->box, node2->box);

  OptNode* n1 = node1;
  if (node1->type == NODE_BOUND) {
    // remove bound node
    n1 = node1->child;
    delete node1;
  }

  OptNode* n2 = node2;
  if (node2->type == NODE_BOUND) {
    // remove bound node
    n2 = node2->child;
    delete node2;
  }

  b->child = n1;
  while (n1->next) { n1 = n1->next; }
  n1->next = n2;
  return b;
}

static OptNode* makeOptNodeList(
  const Scene& s, const std::vector<ObjectPtr>& o_list)
{
  OptNode* node_list = nullptr;
  OptNode* tail = nullptr;
  for (auto& ob : o_list) {
    OptNode* list = nullptr;
    if (auto uPtr = dynamic_cast<const Union*>(ob.get()); uPtr) {
      list = new OptNode(NODE_UNION, ob, uPtr->hitCost(s.hitCosts));
      list->child = makeOptNodeList(s, uPtr->children());
    } else if (auto pPtr = dynamic_cast<const Primitive*>(ob.get()); pPtr) {
      list = new OptNode(NODE_OBJECT, ob, pPtr->hitCost(s.hitCosts));
    } else {
      // assume group - ignore it and just process children
      list = makeOptNodeList(s, ob->children());
      if (!list) { continue; }
    }

    if (tail) { tail->next = list; } else { node_list = tail = list; }
    while (tail->next) { tail = tail->next; }
  }

  return node_list;
}

static void optimizeOptNodeList(const Scene& s, OptNode*& node_list, Flt weight)
{
  // create array to index nodes
  std::vector<OptNode*> node_array;
  node_array.reserve(std::size_t(CountNodes(node_list)));
  const Flt bound_cost = s.hitCosts.bound;

  for (OptNode* ptr = node_list; ptr != nullptr; ) {
    OptNode* n = ptr;
    ptr = ptr->next;
    n->next = nullptr;

    const Flt cost1 = n->cost(weight);
    n->currentCost = cost1;
    const Flt cost2 = (weight * bound_cost) + n->cost(n->box.weight());
    if (cost1 > cost2) {
      // Put object into bound (alone)
      OptNode* b = new OptNode(bound_cost);
      b->child = n;
      b->box   = n->box;
      b->currentCost = cost2;
      n = b;
    }
    node_array.push_back(n);
  }

  // Group objects together in bounding boxes
  auto node_count = node_array.size();
  while (node_count > 1) {
    Flt best = 0, bestMergeCost = 0;
    unsigned int best_i = 0, best_j = 0;

    for (unsigned int i = 0; i < (node_count-1); ++i) {
      OptNode* n1 = node_array[i];

      for (unsigned int j = i+1; j < node_count; ++j) {
        OptNode* n2 = node_array[j];

        const Flt baseCost = n1->currentCost + n2->currentCost;
        const Flt mergeCost = calcMergeCost(s, n1, n2, weight);
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

    OptNode* n = mergeOptNodes(s, node_array[best_i], node_array[best_j]);
    n->currentCost = bestMergeCost;
    node_array[best_i] = n;
    node_array[best_j] = node_array[--node_count];
  }

  // Reconstruct linked list
  node_list = nullptr;
  OptNode* prev = nullptr;
  for (unsigned int i = 0; i < node_count; ++i) {
    OptNode* n = node_array[i];
    if (prev) { prev->next = n; } else { node_list = n; }
    prev = n;

    // Optimize inside bounds/unions
    if (n->child && (n->child->next || n->child->type == NODE_UNION)) {
      // bound/union with 2 or more children
      // - OR - bound containing a single union
      optimizeOptNodeList(s, n->child, n->box.weight());
    }
  }
}

static int convertNodeList(
  OptNode* node_list, std::vector<ObjectPtr>& bound_list, BBox* bound_box)
{
  bound_list.reserve(std::size_t(CountNodes(node_list)));

  int bound_count = 0;
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    if (n->type == NODE_OBJECT) {
      bound_list.push_back(n->object);
      if (bound_box) { bound_box->fit(n->box); }
    } else if (n->type == NODE_UNION) {
      auto u = makeObject<Union>();
      bound_count += convertNodeList(n->child, u->objects, nullptr);
      bound_list.push_back(u);
      if (bound_box) { bound_box->fit(n->box); }
    } else { // n->type == NODE_BOUND
      auto b = makeObject<Bound>();
      bound_count += 1 + convertNodeList(n->child, b->objects, &b->box);
      bound_list.push_back(b);
      if (bound_box) { bound_box->fit(b->box); }
    }
  }

  return bound_count;
}

int MakeBoundList(const Scene& s, const std::vector<ObjectPtr>& o_list,
                  std::vector<ObjectPtr>& bound_list)
{
  OptNode* node_list = makeOptNodeList(s, o_list);
  if (!node_list) {
    return 0;
  }

  BBox box{s.eye};
  for (OptNode* n = node_list; n != nullptr; n = n->next) {
    box.fit(n->box);
  }

  const Flt scene_weight = box.weight();
  println("Old tree cost: ", treeCost(node_list, scene_weight));
  optimizeOptNodeList(s, node_list, scene_weight);
  println("New tree cost: ", treeCost(node_list, scene_weight));

  bound_list.clear();
  const int bound_count = convertNodeList(node_list, bound_list, nullptr);
  killTree(node_list);
  return bound_count;
}
