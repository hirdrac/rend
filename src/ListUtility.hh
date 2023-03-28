//
// ListUtility.hh
// Copyright (C) 2023 Richard Bradley
//
// linked list node template classes & support functions
//
// Node structs/classes must have in their public section:
//  type* next = nullptr;
//

#pragma once


// **** Functions ****
[[nodiscard]] constexpr int CountNodes(const auto* list)
{
  int count = 0;
  while (list) {
    list = list->next;
    ++count;
  }

  return count;
}

void KillNodes(auto* list)
{
  while (list) {
    decltype(list) tmp = list;
    list = list->next;
    delete tmp;
  }
}

[[nodiscard]] auto* LastNode(auto* list)
{
  //assert(item != nullptr);
  while (list->next) { list = list->next; }
  return list;
}
