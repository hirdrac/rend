//
// PatternShaders.cc
// Copyright (C) 2021 Richard Bradley
//

#include "PatternShaders.hh"
#include "Ray.hh"
#include "Print.hh"
#include <cmath>
#include <cassert>


// **** PatternShader Class ****
int PatternShader::addShader(const ShaderPtr& sh, SceneItemFlag flag)
{
  assert(sh != nullptr);
  if (flag == FLAG_BORDER) {
    if (_border) { return -1; }
    _border = sh;
    return 0;
  }

  if (flag != FLAG_NONE) { return -1; }
  _children.push_back(sh);
  return 0;
}

int PatternShader::init(Scene& s)
{
  if (_children.empty()) { return -1; }

  if (_border) {
    if (!IsPositive(_borderwidth)) {
      println("Invalid borderwidth of ", _borderwidth);
      return -1;
    }

    int err = InitShader(s, *_border, &_trans);
    if (err) { return err; }
  }

  for (auto& sh : _children) {
    int err = InitShader(s, *sh, &_trans);
    if (err) { return err; }
  }

  return 0;
}


// **** Checkerboard Class ****
Color Checkerboard::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Vec2 m = _trans.pointLocalToGlobalXY(eh.map, r.time);

  if (_border) {
    const Flt half_bw = _borderwidth * .5;
    if (Abs(m.x - std::floor(m.x + half_bw)) < half_bw
        || Abs(m.y - std::floor(m.y + half_bw)) < half_bw) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c =
    int(std::floor(m.x + VERY_SMALL) + std::floor(m.y + VERY_SMALL));
  return child(c)->evaluate(js, s, r, eh);
}


// **** Checkerboard3D Class ****
Color Checkerboard3D::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Vec3 m = _trans.pointLocalToGlobal(eh.map, r.time);

  if (_border) {
    const Flt half_bw = _borderwidth * .5;
    if (Abs(m.x - std::floor(m.x + half_bw)) < half_bw
        || Abs(m.y - std::floor(m.y + half_bw)) < half_bw
        || Abs(m.z - std::floor(m.z + half_bw)) < half_bw) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c =
    int(std::floor(m.x + VERY_SMALL) + std::floor(m.y + VERY_SMALL)
        + std::floor(m.z + VERY_SMALL));
  return child(c)->evaluate(js, s, r, eh);
}


// **** Pinwheel Class ****
Color Pinwheel::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Vec2 m = _trans.pointLocalToGlobalXY(eh.map, r.time);

  const Flt radius = std::sqrt(Sqr(m.x) + Sqr(m.y));
  const Flt spin_val = _spin * radius * PI * .25;
  const Flt angle = std::atan2(m.y, m.x) + spin_val;

  const Flt sect = Flt(_sectors) * (angle / (PI * 2.0));
  if (_border) {
    // check distance to nearest edge point at the same radius
    // (works well enough for border calc if the spin isn't too high)
    const Flt edge_angle =
      ((std::floor(sect + .5) / Flt(_sectors)) * PI * 2.0) - spin_val;
    const Flt edge_x = radius * std::cos(edge_angle);
    const Flt edge_y = radius * std::sin(edge_angle);
    if ((Sqr(edge_x - m.x) + Sqr(edge_y - m.y)) < Sqr(_borderwidth * .5)) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c = int(std::floor(sect));
  return child(c)->evaluate(js, s, r, eh);
}


// **** Ring Class ****
Color Ring::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Vec2 m = _trans.pointLocalToGlobalXY(eh.map, r.time);
  const Flt d = std::sqrt(Sqr(m.x) + Sqr(m.y)) + _offset;

  if (_border) {
    const Flt half_bw = _borderwidth * .5;
    if ((d - _offset) > half_bw
        && Abs(d - std::floor(d + half_bw)) < half_bw) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c = int(std::floor(d));
  return child(c)->evaluate(js, s, r, eh);
}


// **** SquareRing Class ****
Color SquareRing::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Vec2 m = _trans.pointLocalToGlobalXY(eh.map, r.time);
  const Flt d = std::max(Abs(m.x), Abs(m.y)) + _offset;

  if (_border) {
    const Flt half_bw = _borderwidth * .5;
    if ((d - _offset) > half_bw
        && Abs(d - std::floor(d + half_bw)) < half_bw) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c = int(std::floor(d));
  return child(c)->evaluate(js, s, r, eh);
}


// **** Stripe Class ****
Color Stripe::evaluate(
  JobState& js, const Scene& s, const Ray& r, const EvaluatedHit& eh) const
{
  const Flt d = _trans.pointLocalToGlobalX(eh.map, r.time);

  if (_border) {
    const Flt half_bw = _borderwidth * .5;
    if (Abs(d - std::floor(d + half_bw)) < half_bw) {
      return _border->evaluate(js, s, r, eh);
    }
  }

  const int c = int(std::floor(d + VERY_SMALL));
  return child(c)->evaluate(js, s, r, eh);
}
