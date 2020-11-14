//
// Object.hh - revision 59 (2019/1/4)
// Copyright(C) 2019 by Richard Bradley
//
// Definition of base object class
//

#ifndef Object_hh
#define Object_hh

#include "Transform.hh"
#include "SceneItem.hh"
#include "Types.hh"
#include "SList.hh"
#include <string>


// **** Types ****
class HitInfo;
class HitList;
class Shader;
class Ray;

// Bounding Box class definition
//  simple class for storing object bounding box information
class BBox
{
 public:
  Vec3 pmin, pmax;

  // Constructor
  BBox();

  // Member Functions
  void reset();
  Flt  weight() const;
  void fit(const Vec3& pt);
  void fit(const BBox& box);
};


// Object base class
class Object : public SceneItem, public SListNode<Object>
{
 public:
  // Constructor
  Object();
  // Destructor
  ~Object();

  // SceneItem Functions
  const char* name() const override { return _name.c_str(); }
  int setName(const std::string& str) override { _name = str; return 0; }

  // Member Functions
  virtual int bound(BBox& b) const { return -1; }
  virtual int intersect(const Ray& r, HitList& hit_list) const = 0;
  virtual int evalHit(const HitInfo& h, Vec3& normal, Vec3& map) const {
    return -1; }
  virtual Flt hitCost() const { return 1.0; }
  virtual int setRadius(Flt r) { return -1; }
  virtual bool isVisible() const { return false; }
  virtual bool isSolid() const { return false; }
  virtual int setSolid(bool s) { return -1; }
  virtual Shader* shader() const { return nullptr; }
  virtual int     setShader(Shader* sh) { return -1; }
  virtual Object* childList() const { return nullptr; }

 protected:
  std::string _name;
};


// Primitive base class
//   a visible scene object (sphere, csg, etc.) that can be transformed
class Primitive : public Object
{
 public:
  Flt index = 1.0;

  // SceneItem Functions
  int add(SceneItem* i, SceneItemFlag flag = NO_FLAG) override;
  int init(Scene& s) override;
  Transform*       trans() override       { return &_trans; }
  const Transform* trans() const override { return &_trans; }

  // Object Functions
  int intersect(const Ray& r, HitList& hit_list) const override { return 0; }
  int bound(BBox& b) const override;
  bool isVisible() const override    { return true; }
  bool isSolid() const override      { return _solid; }
  int setSolid(bool s) override      { _solid = s; return 0; }
  Shader* shader() const override    { return _shader; }
  int setShader(Shader* sh) override { _shader = sh; return 0; }

 protected:
  Transform _trans;
  Shader* _shader = nullptr; // not owner
  bool _solid = false;
};


// **** Functions ****
int InitObjectList(
  Scene& s, Object* list, Shader* sh = nullptr, const Transform* t = nullptr);
int MakeBound(BBox& b, const Transform& t, const Vec3 pt_list[], int pt_count);

#endif
