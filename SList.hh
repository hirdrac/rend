//
// SList.hh
// Copyright (C) 2021 Richard Bradley
//
// intrusive single linked list
//
// Example Usage:
// ~~~~~~~~~~~~~
// class Node : public SListNode<Node> { ... };
// SList<Node> list;
// list.addToTail(new Node);
// Node* n = list.removeHead();
//

#pragma once
#include "ListUtility.hh"


// **** SListNode class ****
template<typename type>
class SListNode
{
 public:
  // Member Functions
  [[nodiscard]] type* next() const { return _next; }
  void setNext(type* x) { _next = x; }

 private:
  type* _next = nullptr;
};


// **** SList Class ****
template<typename type>
class SList
{
 public:
  // Constructors
  SList() = default;
  SList(SList<type>&& x) noexcept { swap(x); }
  SList(type* item) : _head(item), _tail(LastNode(item)) { }

  // Destructor
  ~SList() { KillNodes(_head); }

  // Operators
  [[nodiscard]] type* operator[](int i) const { return index(i); }

  // Member Functions
  [[nodiscard]] type* head()       const { return _head; }
  [[nodiscard]] type* tail()       const { return _tail; }
  [[nodiscard]] bool  empty()      const { return !_head; }
  [[nodiscard]] int   count()      const { return CountNodes(_head); }
  [[nodiscard]] type* index(int i) const { return IndexNodes(_head, i); }

  [[nodiscard]] int  findIndex(const type* n) const { return FindNodeIndex(_head, n); }
  [[nodiscard]] bool isNodeHere(const type* n) const { return NodeInList(_head, n); }
  void purge() { KillNodes(_head); _head = nullptr; _tail = nullptr; }

  int addToHead(type* item);
  int addToHead(SList<type>& list);
  int addToTail(type* item);
  int addToTail(SList<type>& list);
  int addAfterNode(type* node, type* item);

  type* removeHead();
  type* removeTail();
  type* removeNext(type* item);
  int   remove(type* node);

  void  chop(int max, int start = 0);
  void  reverse();
  type* extractNodes();

  void swap(SList<type>& x) noexcept {
    std::swap(_head, x._head);
    std::swap(_tail, x._tail);
  }

  // Template Functions
  template<typename fn_type>
  void forEach(fn_type fn) { ForEachNode(_head, fn); }

 private:
  type* _head = nullptr;
  type* _tail = nullptr;

  // prevent copy/assign
  SList(const SList<type>&) = delete;
  SList<type>& operator=(const SList<type>&) = delete;
  SList<type>& operator=(SList<type>&&) = delete;
};

template<typename type>
int SList<type>::addToHead(type* item)
{
  if (!item) { return -1; }

  type* end = LastNode(item);
  end->setNext(_head);
  if (!_tail) { _tail = end; }

  _head = item;
  return 0;
}

template<typename type>
int SList<type>::addToHead(SList<type>& list)
{
  if (list.empty()) { return -1; }

  type* end = list.tail();
  type* start = list.extractNodes();
  end->setNext(_head);
  if (!_tail) { _tail = end; }

  _head = start;
  return 0;
}

template<typename type>
int SList<type>::addToTail(type* item)
{
  if (!item) { return -1; }

  if (_tail) {
    _tail->setNext(item);
  } else {
    _head = item;
  }

  _tail = LastNode(item);
  return 0;
}

template<typename type>
int SList<type>::addToTail(SList<type>& list)
{
  if (list.empty()) { return -1; }

  type* end = list.tail();
  type* start = list.extractNodes();

  if (_tail) {
    _tail->setNext(start);
  } else {
    _head = start;
  }

  _tail = end;
  return 0;
}

template<typename type>
int SList<type>::addAfterNode(type* node, type* item)
{
  if (!node) {
    return addToHead(item);
  } else if (node == _tail) {
    return addToTail(item);
  }

  type* end = LastNode(item);
  end->setNext(node->next());
  node->setNext(item);
  return 0;
}

template<typename type>
type* SList<type>::removeHead()
{
  if (!_head) { return nullptr; }

  type* n = _head;
  _head = _head->next();
  if (!_head) { _tail = nullptr; }

  n->setNext(nullptr);
  return n;
}

template<typename type>
type* SList<type>::removeTail()
{
  if (!_tail) { return nullptr; }

  type* n = _tail;
  if (n == _head) {
    _head = nullptr;
    _tail = nullptr;
    return n;
  }

  type* p = _head;
  while (p->next() != n) { p = p->next(); }

  p->setNext(nullptr);
  _tail = p;
  return n;
}

template<typename type>
type* SList<type>::removeNext(type* item)
{
  if (!item) { return removeHead(); }

  type* n = item->next();
  if (n) {
    item->setNext(n->next());
    if (_tail == n) { _tail = item; }

    n->setNext(nullptr);
  }

  return n;
}

template<typename type>
int SList<type>::remove(type* node)
{
  if (node && _head) {
    if (_head == node) {
      _head = _head->next();
      if (!_head) { _tail = nullptr; }
      node->setNext(nullptr);
      return 0;
    }

    for (type* n = _head; n != nullptr; n = n->next()) {
      if (n->next() == node) {
	n->setNext(node->next());
	if (_tail == node) { _tail = n;	}
	node->setNext(nullptr);
	return 0;
      }
    }
  }

  return -1;
}

template<typename type>
void SList<type>::chop(int max, int start)
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
void SList<type>::reverse()
{
  SList<type> tmp;
  while (!empty()) {
    tmp.addToTail(removeHead());
  }

  swap(*this, tmp);
}

template<typename type>
type* SList<type>::extractNodes()
{
  type* list = _head;
  _head = nullptr;
  _tail = nullptr;
  return list;
}


// **** Functions ****
template<typename type>
void swap(SList<type>& a, SList<type>& b) noexcept { a.swap(b); }
