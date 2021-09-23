//
// BasicObjects.hh
// Copyright (C) 2021 Richard Bradley
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
  int init(Scene& s) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _normal;
};

class Cone final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cone>"; }

  // Object Functions
  int init(Scene& s) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _baseNormal;
};

class Cube final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cube>"; }

  // Object Functions
  int init(Scene& s) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _sideNormal[6];
};

class Cylinder final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Cylinder>"; }

  // Object Functions
  int init(Scene& s) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _endNormal[2];
};

class Paraboloid final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Paraboloid>"; }

  // Object Functions
  int init(Scene& s) override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _baseNormal;
};

class Plane final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Plane>"; }

  // Object Functions
  int init(Scene& s) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Vec3 _normal;
};

class Sphere final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Sphere>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;
};

class Torus final : public Primitive
{
 public:
  // SceneItem Functions
  std::string desc() const override { return "<Torus>"; }
  int setRadius(Flt r) override { _radius = r; return 0; }

  // Object Functions
  int init(Scene& s) override;
  BBox bound(const Matrix* t) const override;
  int intersect(const Ray& r, HitList& hl) const override;

  // Primitive Functions
  Flt hitCost() const override;
  Vec3 normal(const Ray& r, const HitInfo& h) const override;

 private:
  Flt _radius = .5;
};
