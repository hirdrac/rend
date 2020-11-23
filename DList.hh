//
// DList.hh
// Copyright (C) 2020 Richard Bradley
//
// intrusive double linked list
//
// Example Usage:
// ~~~~~~~~~~~~~
// class Node : public DListNode<Node> { ... };
// DList<Node> list;
// list.addToTail(new Node);
// Node* n = list.removeHead();
//

#pragma once
#include "ListUtility.hh"


// **** DListNode class ****
template<typename type>
class DListNode
{
 public:
  // Member Functions
  type* fore() const { return _fore; }
  type* next() const { return _next; }

  void setFore(type* x) { _fore = x; }
  void setNext(type* x) { _next = x; }

 private:
  type* _fore = nullptr;
  type* _next = nullptr;
};


// **** DList class ****
template<typename type>
class DList
{
 public:
  // Constructors
  DList() = default;
  DList(DList<type>&& x) noexcept { swap(x); }
  DList(type* item) : _head(item), _tail(LastNode(item)) { }

  // Destructor
  ~DList() { KillNodes(_head); }

  // Operators
  type* operator[](int i) const { return index(i); }

  // Member Functions
  type* head()       const { return _head; }
  type* tail()       const { return _tail; }
  bool  empty()      const { return !_head; }
  int   count()      const { return CountNodes(_head); }
  type* index(int i) const { return IndexNodes(_head, i); }
  type* reverseIndex(int i) const { return ReverseIndexNodes(_head, i); }

  int  findIndex(const type* n) const { return FindNodeIndex(_head, n); }
  bool isNodeHere(const type* n) const { return NodeInList(_head, n); }

  void purge() { KillNodes(_head); _head = nullptr; _tail = nullptr; }

  int addToHead(type* item);
  int addToHead(DList<type>& list);
  int addToTail(type* item);
  int addToTail(DList<type>& list);
  int addAfterNode(type* node, type* item);
  int addBeforeNode(type* node, type* item);

  type* removeHead();
  type* removeTail();
  int   remove(type* node);
  int   safeRemove(type* node);

  void  chop(int max, int start = 0);
  void  reverse();
  type* extractNodes();

  void swap(DList<type>& x) noexcept {
    std::swap(_head, x._head);
    std::swap(_tail, x._tail);
  }

  // Template Functions
  template<typename fn_type>
  void forEach(fn_type fn) { ForEachNode(_head, fn); }

  template<typename cmp_fn>
  int insertFromHead(type* item, cmp_fn cmp)
  {
    type* n = _head;
    while (n) {
      if (cmp(n, item) >= 0) { break; }
      n = n->next();
    }

    return addBeforeNode(n, item);
  }

  template<typename cmp_fn>
  int insertFromTail(type* item, cmp_fn cmp)
  {
    type* n = _tail;
    while (n) {
      if (cmp(n, item) <= 0) { break; }
      n = n->fore();
    }

    return addAfterNode(n, item);
  }

  template<typename cmp_fn>
  int sort(cmp_fn cmp)
  {
    if (!_head) { return 0; }

    type* pos  = _head;
    type* next = pos->next();
    while (next) {
      if (node_cmp(pos, next) > 0) {
	remove(next);
	insertFromHead(next, cmp);
      } else {
	pos = next;
      }

      next = pos->next();
    }

    return 0;
  }

 private:
  type* _head = nullptr;
  type* _tail = nullptr;

  // prevent copy/assign
  DList(const DList<type>&) = delete;
  DList<type>& operator=(const DList<type>&) = delete;
  DList<type>& operator=(DList<type>&&) = delete;
};

template<typename type>
int DList<type>::addToHead(type* item)
{
  if (!item) { return -1; }

  type* end = LastNode(item);
  end->setNext(_head);
  if (_head) {
    _head->setFore(end);
  } else {
    _tail = end;
  }

  _head = item;
  return 0;
}

template<typename type>
int DList<type>::addToHead(DList<type>& list)
{
  if (list.empty()) { return -1; }

  type* end = list.tail();
  type* start = list.extractNodes();
  end->setNext(_head);
  if (_head) {
    _head->setFore(end);
  } else {
    _tail = end;
  }

  _head = start;
  return 0;
}

template<typename type>
int DList<type>::addToTail(type* item)
{
  if (!item) { return -1; }

  item->setFore(_tail);
  if (_tail) {
    _tail->setNext(item);
  } else {
    _head = item;
  }

  _tail = LastNode(item);
  return 0;
}

template<typename type>
int DList<type>::addToTail(DList<type>& list)
{
  if (list.empty()) { return -1; }

  type* end = list.tail();
  type* start = list.extractNodes();

  start->setFore(_tail);
  if (_tail) {
    _tail->setNext(start);
  } else {
    _head = start;
  }

  _tail = end;
  return 0;
}

template<typename type>
int DList<type>::addAfterNode(type* node, type* item)
{
  if (!node) {
    return addToHead(item);
  } else if (node == _tail) {
    return addToTail(item);
  }

  type* end = LastNode(item);
  item->setFore(node);
  end->setNext(node->next());
  node->next()->setFore(end);
  node->setNext(item);
  return 0;
}

template<typename type>
int DList<type>::addBeforeNode(type* node, type* item)
{
  if (!node) {
    return addToTail(item);
  } else if (node == _head) {
    return addToHead(item);
  }

  type* end = LastNode(item);
  end->setNext(node);
  item->setFore(node->fore());
  item->fore()->setNext(item);
  node->setFore(end);
  return 0;
}

template<typename type>
type* DList<type>::removeHead()
{
  if (!_head) { return nullptr; }

  type* node = _head;
  _head = _head->next();
  if (_head) {
    _head->setFore(nullptr);
  } else {
    _tail = nullptr;
  }

  node->setNext(nullptr);
  return node;
}

template<typename type>
type* DList<type>::removeTail()
{
  if (!_tail) { return nullptr; }

  type* node = _tail;
  _tail = _tail->fore();
  if (_tail) {
    _tail->setNext(nullptr);
  } else {
    _head = nullptr;
  }

  node->setFore(nullptr);
  return node;
}

template<typename type>
int DList<type>::remove(type* node)
{
  if (!node) { return -1; }

  if (_head == node) {
    _head = node->next();
  }

  if (_tail == node) {
    _tail = node->fore();
  }

  if (node->next()) {
    node->next()->setFore(node->fore());
  }

  if (node->fore()) {
    node->fore()->setNext(node->next());
  }

  node->setFore(nullptr);
  node->setNext(nullptr);
  return 0;
}

template<typename type>
int DList<type>::safeRemove(type* node)
{
  return isNodeHere(node) ? remove(node) : -1;
}

template<typename type>
void DList<type>::chop(int max, int start)
{
  while (start > 0) {
    delete removeHead();
    --start;
  }

  type* tmp = index(max - 1);
  if (tmp && tmp->next()) {
    KillNodes(tmp->next());
    tmp->setNext(nullptr);
    _tail = tmp;
  }
}

template<typename type>
void DList<type>::reverse()
{
  DList<type> tmp;
  while (!empty()) {
    tmp.addToTail(removeHead());
  }

  swap(*this, tmp);
}

template<typename type>
type* DList<type>::extractNodes()
{
  type* list = _head;
  _head = nullptr;
  _tail = nullptr;
  return list;
}


// **** Functions ****
template<typename type>
void swap(DList<type>& a, DList<type>& b) noexcept { a.swap(b); }
