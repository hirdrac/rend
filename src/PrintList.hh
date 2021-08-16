//
// PrintList.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include "Print.hh"


template<class ContainerType>
void PrintList(const ContainerType& list, int indent = 0)
{
  if (list.empty()) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println("*EMPTY*");
    return;
  }

  for (auto& x : list) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println(x->desc());
    if (!(x->children().empty())) {
      PrintList(x->children(), indent + 2);
    }
  }
}
