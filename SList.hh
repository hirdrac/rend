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
#include <utility>


// **** SListNode class ****
template<typename type>
class SListNode
{
 public:
  // Member Functions
  [[nodiscard]] type* next() { return _next; }
  [[nodiscard]] const type* next() const { return _next; }
  void setNext(type* x) { _next = x; }

 private:
  type* _next = nullptr;
};


// **** SList class ****
template<typename type>
class SList
{
 public:
  SList() = default;
  SList(type* item) : _head(item), _tail(LastNode(item)) { }
  ~SList() { KillNodes(_head); }

  // prevent copy/assign
  SList(const SList<type>&) = delete;
  SList<type>& operator=(const SList<type>&) = delete;

  // allow move/move-assign
  SList(SList<type>&& x) noexcept :
    _head(std::exchange(x._head, nullptr)),
    _tail(std::exchange(x._tail, nullptr)) { }

  SList<type>& operator=(SList<type>&& x) noexcept {
    if (this != &x) {
      KillNodes(_head);
      _head = std::exchange(x._head, nullptr);
      _tail = std::exchange(x._tail, nullptr);
    }
    return *this;
  }


  // Member Functions
  [[nodiscard]] type* head() { return _head; }
  [[nodiscard]] const type* head() const { return _head; }

  [[nodiscard]] type* tail() { return _tail; }
  [[nodiscard]] const type* tail() const { return _tail; }

  [[nodiscard]] bool empty() const { return !_head; }
  [[nodiscard]] int count() const { return CountNodes(_head); }

  void purge() { KillNodes(_head); _head = nullptr; _tail = nullptr; }

  int addToHead(type* item);
  int addToHead(SList<type>& list);
  int addToTail(type* item);
  int addToTail(SList<type>& list);
  int addAfterNode(type* node, type* item);

  type* removeHead();
  type* removeTail();
  type* removeNext(type* item);

  type* extractNodes();

  void swap(SList<type>& x) noexcept {
    std::swap(_head, x._head);
    std::swap(_tail, x._tail);
  }

 private:
  type* _head = nullptr;
  type* _tail = nullptr;
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
