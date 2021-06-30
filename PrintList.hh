//
// PrintList.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Print.hh"


template<class T>
void PrintList(const T* list, int indent = 0)
{
  if (!list) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println("*EMPTY*");
    return;
  }

  for (; list != nullptr; list = list->next()) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println(list->desc());
    if (list->childList()) {
      PrintList(list->childList(), indent + 2);
    }
  }
}
