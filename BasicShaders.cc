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

int ShaderColor::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  result = _color;
  return 0;
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

int ShaderGlobal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, eh.global_pt};
  return _child->evaluate(s, r, h, eh2, result);
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

int ShaderLocal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  EvaluatedHit eh2{eh.global_pt, eh.normal, h.local_pt};
  return _child->evaluate(s, r, h, eh2, result);
}


// **** Checkerboard Class ****
int Checkerboard::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  Vec3 m = MultPoint(eh.map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x));
  int gy = int(std::floor(m.y));
  int gz = int(std::floor(m.z));
  int x  = Abs(gx + gy + gz) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh, result);
}


// **** ColorCube Class ****
int ColorCube::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  Vec3 m = MultPoint(eh.map, _trans.GlobalInv(r.time));
  result = {
    static_cast<Color::value_type>(Abs(m.x)),
    static_cast<Color::value_type>(Abs(m.y)),
    static_cast<Color::value_type>(Abs(m.z))
  };
  return 0;
}


// **** Ring Class ****
int Ring::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  Vec3 m = MultPoint(eh.map, _trans.GlobalInv(r.time));
  int d = int(std::sqrt(Sqr(m.x) + Sqr(m.y) + Sqr(m.z)) * 2.0);
  int x = Abs(d) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh, result);
}


// **** ShaderSide Class ****
int ShaderSide::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  int x = h.side % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh, result);
}


// **** Stripe Class ****
int Stripe::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const EvaluatedHit& eh,
  Color& result) const
{
  Vec3 m = MultPoint(eh.map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x));
  int x  = Abs(gx) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, eh, result);
}
