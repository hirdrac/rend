//
// BasicObjects.hh - revision 24 (2019/1/3)
// Copyright(C) 2019 by Richard Bradley
//
// Definitions for primitive object classes
//

#ifndef BasicObjects_hh
#define BasicObjects_hh

#include "Object.hh"
#include "HitCostInfo.hh"
#include <iostream>


// **** Types ****
class Disc : public Primitive
{
 public:
  // Constructor
  Disc();

  // SceneItem Functions
  void print(std::ostream& out, int) const override { out << "<Disc>"; }
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
  void print(std::ostream& out, int) const override { out << "<Cone>"; }
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
  void print(std::ostream& out, int) const override { out << "<Cube>"; }
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
  void print(std::ostream& out, int) const override { out << "<Cylinder>"; }
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
  void print(std::ostream& out, int) const override { out << "<OpenCone>"; }

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
  void print(std::ostream& out, int) const override { out << "<OpenCylinder>"; }

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
  void print(std::ostream& out, int) const override { out << "<Paraboloid>"; }

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
  void print(std::ostream& out, int) const override { out << "<Plane>"; }
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
  void print(std::ostream& out, int) const override { out << "<Sphere>"; }

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
  void print(std::ostream& out, int) const override { out << "<Torus>"; }
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

#endif
