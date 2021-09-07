//
// PatternShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "PatternShaders.hh"
#include "Ray.hh"
#include <cmath>
#include <cassert>


// **** PatternShader Class ****
int PatternShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (flag != FLAG_NONE) { return -1; }

  _children.push_back(sh);
  return 0;
}

int PatternShader::init(Scene& s)
{
  if (_children.empty()) { return -1; }

  for (auto& sh : _children) {
    int err = InitShader(s, *sh, &_trans);
    if (err) { return err; }
  }

  return 0;
}


// **** Checkerboard Class ****
Color Checkerboard::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int gx = int(std::floor(m.x));
  int gy = int(std::floor(m.y));
  int gz = int(std::floor(m.z));
  int x  = Abs(gx + gy + gz) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, eh);
}


// **** Ring Class ****
Color Ring::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int d = int(std::sqrt(Sqr(m.x) + Sqr(m.y) + Sqr(m.z)) * 2.0);
  int x = Abs(d) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, eh);
}


// **** Stripe Class ****
Color Stripe::evaluate(
  const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int gx = int(std::floor(m.x));
  int x  = Abs(gx) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, eh);
}
