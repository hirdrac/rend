//
// ListUtility.hh
// Copyright (C) 2022 Richard Bradley
//
// linked list node template classes & support functions
//
// Node structs/classes must have in their public section:
//  type* next = nullptr;
//

#pragma once


// **** Functions ****
template<typename type>
[[nodiscard]] int CountNodes(const type* list)
{
  int count = 0;
  while (list) {
    list = list->next;
    ++count;
  }

  return count;
}

template<typename type>
void KillNodes(type* list)
{
  while (list) {
    type* tmp = list;
    list = list->next;
    delete tmp;
  }
}

template<typename type>
[[nodiscard]] type* LastNode(type* list)
{
  //assert(item != nullptr);
  while (list->next) { list = list->next; }
  return list;
}
