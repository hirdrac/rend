//
// ListUtility.hh
// Copyright (C) 2021 Richard Bradley
//
// linked list node template classes & support functions
//
// Node classes must implement:
//  type* next() const    - returns node following this one
//  void setNext(type* n) - set pointer to node following current node
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
