//
// ListUtility.hh
// Copyright (C) 2021 Richard Bradley
//
// linked list node template classes & support functions
//
// Node classes must implement:
//  type* next() const - returns node following this one
//  type* fore() const - return previous node (double linked lists only)
//
//  void setNext(type* n) - set pointer to node following current node
//  void setFore(type* n) - set pointer to node preceding current node
//

#pragma once


// **** Functions ****
template<typename type>
[[nodiscard]] int CountNodes(const type* list)
{
  int count = 0;
  while (list) {
    list = list->next();
    ++count;
  }

  return count;
}

template<typename type>
void KillNodes(type* list)
{
  while (list) {
    type* tmp = list;
    list = list->next();
    delete tmp;
  }
}

template<typename type>
[[nodiscard]] type* LastNode(type* list)
{
  if (list) { while (list->next()) { list = list->next(); } }
  return list;
}

template<typename type>
[[nodiscard]] const type* LastNode(const type* list)
{
  if (list) { while (list->next()) { list = list->next(); } }
  return list;
}

template<typename type>
[[nodiscard]] type* IndexNodes(type* list, int index)
{
  if (index < 0) { return nullptr; }

  while (list != nullptr && index != 0) {
    list = list->next();
    --index;
  }

  return list;
}

template<typename type>
[[nodiscard]] type* ReverseIndexNodes(type* list, int index)
{
  if (index < 0) { return nullptr; }

  while (list != nullptr && index != 0) {
    list = list->fore();
    --index;
  }

  return list;
}

template<typename type>
[[nodiscard]] bool NodeInList(const type* list, const type* node)
{
  while (list) {
    if (list == node) { return true; } // found
    list = list->next();
  }

  return false; // not found
}

template<typename type>
[[nodiscard]] int FindNodeIndex(const type* list, const type* node)
{
  if (list) {
    int i = 0;
    do {
      if (list == node) { return i; } // found - return index

      ++i;
      list = list->next();
    } while (list);
  }

  return -1; // not found - return error
}

template<typename type, class fn_type>
void ForEachNode(type* list, fn_type fn)
{
  while (list) {
    type* next = list->next();
    fn(list);
    list = next;
  }
}
