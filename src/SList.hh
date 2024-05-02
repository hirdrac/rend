//
// SList.hh
// Copyright (C) 2024 Richard Bradley
//
// Intrusive single linked list
//
// Example Usage:
// ~~~~~~~~~~~~~
// class Node { public: Node* next = nullptr;  ... };
// SList<Node> list;
// list.addToTail(new Node);
// Node* n = list.removeHead();
//

#pragma once
#include "ListUtility.hh"
#include <utility>


// **** SList class ****
template<ListNode T>
class SList
{
 public:
  using type = SList<T>;

  SList() = default;
  explicit SList(T* item) : _head{item}, _tail{LastNode(item)} { }
  ~SList() { KillNodes(_head); }

  // prevent copy/assign
  SList(const type&) = delete;
  type& operator=(const type&) = delete;

  // allow move/move-assign
  SList(type&& x) noexcept :
    _head{std::exchange(x._head, nullptr)},
    _tail{std::exchange(x._tail, nullptr)} { }

  type& operator=(type&& x) noexcept {
    if (this != &x) {
      KillNodes(_head);
      _head = std::exchange(x._head, nullptr);
      _tail = std::exchange(x._tail, nullptr);
    }
    return *this;
  }


  // Member Functions
  [[nodiscard]] T* head() { return _head; }
  [[nodiscard]] const T* head() const { return _head; }

  [[nodiscard]] T* tail() { return _tail; }
  [[nodiscard]] const T* tail() const { return _tail; }

  [[nodiscard]] bool empty() const { return !_head; }
  [[nodiscard]] int count() const { return CountNodes(_head); }

  void purge() { KillNodes(_head); _head = _tail = nullptr; }

  void addToHead(T* item);
  void addToHead(type& list);
  void addToTail(T* item);
  void addToTail(type& list);
  void addAfterNode(T* node, T* item);

  T* removeHead();
  T* removeNext(T* item);
  T* extractNodes();

  void swap(type& x) noexcept {
    std::swap(_head, x._head);
    std::swap(_tail, x._tail);
  }

 private:
  T* _head = nullptr;
  T* _tail = nullptr;
};

template<ListNode T>
void SList<T>::addToHead(T* item)
{
  //assert(item != nullptr);
  T* end = LastNode(item);
  end->next = _head;
  if (!_tail) { _tail = end; }

  _head = item;
}

template<ListNode T>
void SList<T>::addToHead(type& list)
{
  if (list.empty()) { return; }

  T* end = list.tail();
  T* start = list.extractNodes();
  end->next = _head;
  if (!_tail) { _tail = end; }

  _head = start;
}

template<ListNode T>
void SList<T>::addToTail(T* item)
{
  //assert(item != nullptr);
  if (_tail) {
    _tail->next = item;
  } else {
    _head = item;
  }

  _tail = LastNode(item);
}

template<ListNode T>
void SList<T>::addToTail(type& list)
{
  if (list.empty()) { return; }

  T* end = list.tail();
  T* start = list.extractNodes();

  if (_tail) {
    _tail->next = start;
  } else {
    _head = start;
  }

  _tail = end;
}

template<ListNode T>
void SList<T>::addAfterNode(T* node, T* item)
{
  if (!node) {
    addToHead(item);
  } else if (node == _tail) {
    addToTail(item);
  } else {
    //assert(item != nullptr);
    LastNode(item)->next = node->next;
    node->next = item;
  }
}

template<ListNode T>
T* SList<T>::removeHead()
{
  if (!_head) { return nullptr; }

  T* n = _head;
  _head = _head->next;
  if (!_head) { _tail = nullptr; }

  n->next = nullptr;
  return n;
}

template<ListNode T>
T* SList<T>::removeNext(T* item)
{
  if (!item) { return removeHead(); }

  T* n = item->next;
  if (n) {
    item->next = n->next;
    if (_tail == n) { _tail = item; }

    n->next = nullptr;
  }

  return n;
}

template<ListNode T>
T* SList<T>::extractNodes()
{
  T* list = _head;
  _head = _tail = nullptr;
  return list;
}


// **** Functions ****
template<ListNode T>
void swap(SList<T>& a, SList<T>& b) noexcept { a.swap(b); }
