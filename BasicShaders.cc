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

int ShaderColor::init(Scene& s)
{
  _color *= value;
  value = 1.0;
  return 0;
}

int ShaderColor::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  result = _color;
  return 0;
}


// **** ShaderGlobal Class ****
int ShaderGlobal::addShader(Shader* sh, SceneItemFlag flag)
{
  if (!sh || _child) { return -1; }

  _child.reset(sh);
  return 0;
}

int ShaderGlobal::init(Scene& s)
{
  return _child ? InitShader(s, *_child, value) : -1;
}

int ShaderGlobal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  return _child->evaluate(s, r, h, normal, h.global_pt, result);
}


// **** ShaderLocal Class ****
int ShaderLocal::addShader(Shader* sh, SceneItemFlag flag)
{
  if (!sh || _child) { return -1; }

  _child.reset(sh);
  return 0;
}

int ShaderLocal::init(Scene& s)
{
  return _child ? InitShader(s, *_child, value) : -1;
}

int ShaderLocal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  return _child->evaluate(s, r, h, normal, h.local_pt, result);
}


// **** Checkerboard Class ****
int Checkerboard::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x));
  int gy = int(std::floor(m.y));
  int gz = int(std::floor(m.z));
  int x  = Abs(gx + gy + gz) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, normal, map, result);
}


// **** ColorCube Class ****
int ColorCube::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  result.setRGB(Abs(m.x), Abs(m.y), Abs(m.z));
  return 0;
}


// **** Ring Class ****
int Ring::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int d = int(std::sqrt(Sqr(m.x) + Sqr(m.y) + Sqr(m.z)) * 2.0);
  int x = Abs(d) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, normal, map, result);
}


// **** ShaderSide Class ****
int ShaderSide::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  int x = h.side % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, normal, map, result);
}


// **** Stripe Class ****
int Stripe::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x));
  int x  = Abs(gx) % int(_children.size());
  return _children[std::size_t(x)]->evaluate(s, r, h, normal, map, result);
}


#if 0
// **** TextureMap Class ****
int TextureMap::init(Scene& s)
{
  _trans.init();
  sx = (Flt(std::max(fb->getWidth(), fb->getHeight()))) / 2.0;
  sy = sx;
  return 0;
}

int TextureMap::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m;
  MultOptimizedPoint(map, _trans.GlobalInv(r.time), m);
  int fbw = fb->getWidth();
  int fbh = fb->getHeight();

  int x = (int(sx * (m.x + 1.0) + .5)) % fbw;
  int y = (int(sy * (m.y + 1.0) + .5)) % fbh;

  if (x < 0) {
    x += fbw;
  }

  if (y < 0) {
    y += fbh;
  }

  Flt cr, cg, cb;
  fb->Value(x, y, cr, cg, cb);
  result.SetRGB(cr, cg, cb);
  result *= value;
  return 0;
}
#endif
