//
// BasicShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicShaders.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include <sstream>
#include <cmath>


// **** ShaderColor Class ****
std::string ShaderColor::desc() const
{
  std::ostringstream os;
  os << "<Color " << _color << '>';
  return os.str();
}

Color ShaderColor::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  return _color;
}


// **** ShaderGlobal Class ****
int ShaderGlobal::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || _child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int ShaderGlobal::init(Scene& s)
{
  return _child ? InitShader(s, *_child) : -1;
}

Color ShaderGlobal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt};
  return _child->evaluate(s, r, h, eh2);
}


// **** ShaderLocal Class ****
int ShaderLocal::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  if (!sh || _child || flag != FLAG_NONE) { return -1; }

  _child = sh;
  return 0;
}

int ShaderLocal::init(Scene& s)
{
  return _child ? InitShader(s, *_child) : -1;
}

Color ShaderLocal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, h.local_pt};
  return _child->evaluate(s, r, h, eh2);
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
