//
// BasicObjects.hh
// Copyright (C) 2022 Richard Bradley
//
// Definitions for primitive object classes
//

#pragma once
#include "Object.hh"


// **** Types ****
class Disc final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Disc>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _normal{INIT_NONE};
};

class Cone final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cone>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _baseNormal{INIT_NONE};
};

class Cube final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cube>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _sideNormal[6] = {
    INIT_NONE,INIT_NONE,INIT_NONE,INIT_NONE,INIT_NONE,INIT_NONE};
};

class Cylinder final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cylinder>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _endNormal[2] = {INIT_NONE,INIT_NONE};
};

class Paraboloid final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Paraboloid>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _baseNormal{INIT_NONE};
};

class Plane final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Plane>"; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _normal{INIT_NONE};
};

class Sphere final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Sphere>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;
};

class Torus final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Torus>"; }
  int setRadius(Flt r) override { _radius = r; return 0; }

  // Object Functions
  int init(Scene& s, const Transform* tr) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost(const HitCostInfo& hc) const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Flt _radius = .5;
};
