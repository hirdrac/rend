//
// ListUtil.hh
// Copyright (C) 2024 Richard Bradley
//
// Intrusive linked list node support functions
//
// Node structs/classes must have in their public section:
//  type* next = nullptr;
//

#pragma once
#include <concepts>


// **** Concepts ****
template<class T>
concept ListNode = requires(T t) {
  { t.next } -> std::same_as<T*&>;
};


// **** Functions ****
template<ListNode T>
[[nodiscard]] constexpr int CountNodes(const T* list)
{
  int count = 0;
  while (list) {
    list = list->next;
    ++count;
  }

  return count;
}

template<ListNode T>
constexpr void KillNodes(T* list)
{
  while (list) {
    decltype(list) tmp = list;
    list = list->next;
    delete tmp;
  }
}

template<ListNode T>
[[nodiscard]] constexpr T* LastNode(T* list)
{
  //assert(item != nullptr);
  while (list->next) { list = list->next; }
  return list;
}
