//
// BasicObjects.hh
// Copyright (C) 2021 Richard Bradley
//
// Definitions for primitive object classes
//

#pragma once
#include "Object.hh"
#include "HitCostInfo.hh"


// **** Types ****
class Disc : public Primitive
{
 public:
  // Constructor
  Disc();

  // SceneItem Functions
  std::string desc(int) const override { return "<Disc>"; }
  int init(Scene& s) override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  int bound(BBox& b) const override;
  Flt hitCost() const override { return CostTable.disc; }

 private:
  Vec3 normal_cache;
};

class Cone : public Primitive
{
 public:
  // Constructor
  Cone();

  // SceneItem Functions
  std::string desc(int) const override { return "<Cone>"; }
  int init(Scene& s) override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.cone; }

 private:
  Vec3 normal_cache;
};

class Cube : public Primitive
{
 public:
  // Constructor
  Cube();

  // SceneItem Functions
  std::string desc(int) const override { return "<Cube>"; }
  int init(Scene& s) override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.cube; }

 private:
  Vec3 normal_cache[6];
};

class Cylinder : public Primitive
{
 public:
  // Constructor
  Cylinder();

  // SceneItem Functions
  std::string desc(int) const override { return "<Cylinder>"; }
  int init(Scene& s) override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.cylinder; }

 private:
  Vec3 normal_cache[2];
};

class OpenCone : public Primitive
{
 public:
  // Constructor
  OpenCone();

  // SceneItem Functions
  std::string desc(int) const override { return "<OpenCone>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.open_cone; }
};

class OpenCylinder : public Primitive
{
 public:
  // Constructor
  OpenCylinder();

  // SceneItem Functions
  std::string desc(int) const override { return "<OpenCylinder>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.open_cylinder; }
};

class Paraboloid : public Primitive
{
 public:
  // Constructor
  Paraboloid();

  // SceneItem Functions
  std::string desc(int) const override { return "<Paraboloid>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.paraboloid; }
};

class Plane : public Primitive
{
 public:
  // Constructor
  Plane();

  // SceneItem Functions
  std::string desc(int) const override { return "<Plane>"; }
  int init(Scene& s) override;

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  int bound(BBox& b) const override;
  Flt hitCost() const override { return CostTable.plane; }

 private:
  Vec3 normal_cache;
};

class Sphere : public Primitive
{
 public:
  // Constructor
  Sphere();

  // SceneItem Functions
  std::string desc(int) const override { return "<Sphere>"; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.sphere; }
};

class Torus : public Primitive
{
 public:
  // Constructor
  Torus();

  // SceneItem Functions
  std::string desc(int) const override { return "<Torus>"; }
  int init(Scene& s) override;

  // Object Functions
  int bound(BBox& b) const override;
  int intersect(const Ray& r, HitList& hit_list) const override;
  int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const override;
  Flt hitCost() const override { return CostTable.torus; }
  int setRadius(Flt r) override { radius = r; return 0; }

 private:
  Flt radius, r_constant;
};
