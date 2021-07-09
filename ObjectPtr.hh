//
// ObjectPtr.hh
// Copyright (C) 2021 Richard Bradley
//

#pragma once
#include <memory>

class Object;
using ObjectPtr = std::shared_ptr<Object>;

template<class T, typename... Args>
inline std::shared_ptr<T> makeObject(Args... args) {
  return std::make_shared<T>(args...);
}
