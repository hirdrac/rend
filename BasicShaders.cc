//
// BasicShaders.cc
// Copyright (C) 2020 Richard Bradley
//

#include "BasicShaders.hh"
#include "Intersect.hh"
#include "Ray.hh"
#include <cmath>


// **** ShaderColor Class ****
int ShaderColor::init(Scene& s)
{
  color *= value;
  value = 1.0;
  return 0;
}

int ShaderColor::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  result = color;
  return 0;
}


// **** ShaderGlobal Class ****
int ShaderGlobal::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh) { return -1; }

  children.push_back(sh);
  return 0;
}

int ShaderGlobal::init(Scene& s)
{
  for (Shader* sh : children) {
    int err = InitShader(s, *sh, value);
    if (err) { return err; }
  }
  return 0;
}

int ShaderGlobal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  if (children.empty()) { return -1; }
  return children.front()->evaluate(s, r, h, normal, h.global_pt, result);
}


// **** ShaderLocal Class ****
int ShaderLocal::add(SceneItem* i, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(i);
  if (!sh) { return -1; }

  children.push_back(sh);
  return 0;
}

int ShaderLocal::init(Scene& s)
{
  for (Shader* sh : children) {
    int err = InitShader(s, *sh, value);
    if (err) { return err; }
  }
  return 0;
}

int ShaderLocal::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  if (children.empty()) { return -1; }
  return children.front()->evaluate(s, r, h, normal, h.local_pt, result);
}


// **** Checkerboard Class ****
int Checkerboard::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x + VERY_SMALL) + 1.0);
  int gy = int(std::floor(m.y + VERY_SMALL) + 1.0);
  int gz = int(std::floor(m.z + VERY_SMALL) + 1.0);
  int x  = std::abs(gx + gy + gz) % children.size();
  return children[x]->evaluate(s, r, h, normal, map, result);
}


// **** ColorCube Class ****
int ColorCube::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  result.setRGB(std::abs(m.x), std::abs(m.y), std::abs(m.z));
  return 0;
}


// **** Ring Class ****
int Ring::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int d = int(std::sqrt(Sqr(m.x) + Sqr(m.y) + Sqr(m.z)) * 2.0);
  int x = std::abs(d) % children.size();
  return children[x]->evaluate(s, r, h, normal, map, result);
}


// **** ShaderSide Class ****
int ShaderSide::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  int x = h.side % children.size();
  return children[x]->evaluate(s, r, h, normal, map, result);
}


// **** Stripe Class ****
int Stripe::evaluate(
  const Scene& s, const Ray& r, const HitInfo& h, const Vec3& normal,
  const Vec3& map, Color& result) const
{
  Vec3 m = MultPoint(map, _trans.GlobalInv(r.time));
  int gx = int(std::floor(m.x + VERY_SMALL) + 1.0);
  int x  = std::abs(gx) % children.size();
  return children[x]->evaluate(s, r, h, normal, map, result);
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