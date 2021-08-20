//
// PatternShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "PatternShaders.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include <cmath>


// **** PatternShader Class ****
int PatternShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || flag != FLAG_NONE) { return -1; }

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

  _trans.init();
  return 0;
}


// **** Checkerboard Class ****
Color Checkerboard::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int gx = int(std::floor(m.x));
  int gy = int(std::floor(m.y));
  int gz = int(std::floor(m.z));
  int x  = Abs(gx + gy + gz) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh);
}


// **** ColorCube Class ****
Color ColorCube::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  return {
    static_cast<Color::value_type>(Abs(m.x)),
    static_cast<Color::value_type>(Abs(m.y)),
    static_cast<Color::value_type>(Abs(m.z))
  };
}


// **** Ring Class ****
Color Ring::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int d = int(std::sqrt(Sqr(m.x) + Sqr(m.y) + Sqr(m.z)) * 2.0);
  int x = Abs(d) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh);
}


// **** ShaderSide Class ****
Color ShaderSide::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  int x = h.side % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh);
}


// **** Stripe Class ****
Color Stripe::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);
  int gx = int(std::floor(m.x));
  int x  = Abs(gx) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh);
}
