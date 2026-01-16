//
// PrintList.hh
// Copyright (C) 2026 Richard Bradley
//

#pragma once
#include "Print.hh"


void printList(const auto& list, int indent = 0)
{
  if (list.empty()) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println("*EMPTY*");
    return;
  }

  for (auto& x : list) {
    for (int i = 0; i < indent; ++i) { print(' '); }
    println(x->desc());
    const auto children = x->children();
    if (!(children.empty())) {
      printList(children, indent + 2);
    }
  }
}
