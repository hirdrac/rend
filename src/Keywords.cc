//
// Keywords.cc
// Copyright (C) 2022 Richard Bradley
//

#include "Keywords.hh"
#include "RegisterKeyword.hh"
#include "Parser.hh"
#include "Scene.hh"
#include "BasicLights.hh"
#include "BasicObjects.hh"
#include "ColorShaders.hh"
#include "PatternShaders.hh"
#include "MapShaders.hh"
#include "NoiseShaders.hh"
#include "Occlusion.hh"
#include "CSG.hh"
#include "Phong.hh"
#include "Group.hh"
#include "Prism.hh"
#include "BBox.hh"
#include "Print.hh"
#include <map>
#include <memory>
#include <cctype>
#include <cassert>


// **** Helper Functions ****
static Transform* findTrans(SceneItem* p)
{
  if (!p) { return nullptr; }
  Transform* t = p->trans();
  if (!t) { println_err("ERROR: ", p->desc(), " cannot be transformed"); }
  return t;
}

static bool notDone(SceneParser& sp, AstNode* n)
{
  if (n == nullptr) { return false; }

  sp.reportError(n, "Unexpected value '", n->val, "'");
  return true;
}


// **** Item Functions ****
// item transforms
static int MoveFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (sp.getVec3(n, v) || notDone(sp, n)) { return -1; }

  t->base.translate(v);
  return 0;
}

template<BBox::Spot spot>
static int MoveByBBoxSpotFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  Object* ob = dynamic_cast<Object*>(p);
  if (!t || !ob) { return -1; }

  Vec3 v;
  if (sp.getVec3(n, v) || notDone(sp, n)) { return -1; }

  v -= ob->bound(&t->base)(spot);
  t->base.translate(v);
  return 0;
}

static int NoParentFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t || notDone(sp, n)) { return -1; }

  t->setNoParent(true);
  return 0;
}

template<Matrix::Axis axis>
static int RotateByAxisFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (sp.getFlt(n, angle) || notDone(sp, n)) { return -1; }

  t->base.rotate<axis>(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

static int ScaleFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (sp.getVec3(n, v) || notDone(sp, n)) { return -1; }

  t->base.scale(v);
  return 0;
}

template<Matrix::Axis axis>
static int ScaleAxisFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt v;
  if (sp.getFlt(n, v) || notDone(sp, n)) { return -1; }

  t->base.scale<axis>(v);
  return 0;
}

template<Matrix::Axis axis1, Matrix::Axis axis2>
static int Scale2AxesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt v;
  if (sp.getFlt(n, v) || notDone(sp, n)) { return -1; }

  t->base.scale<axis1>(v);
  t->base.scale<axis2>(v);
  return 0;
}

static int ScaleAllAxesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt v;
  if (sp.getFlt(n, v) || notDone(sp, n)) { return -1; }

  t->base.scale(v, v, v);
  return 0;
}

static int StretchXFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  // stretch object along its X axis between 2 points
  Transform* t = findTrans(p);
  Object* ob = dynamic_cast<Object*>(p);
  if (!t || !ob) { return -1; }

  Vec3 p1, p2;
  if (sp.getVec3(n, p1) || sp.getVec3(n, p2) || notDone(sp, n)) { return -1; }

  const Vec3 dir = p2 - p1;
  const Flt len = dir.length();
  if (IsZero(len)) {
    println_err("Invalid stretch length");
    return -1;
  }

  const Vec3 center = (p1+p2) * .5;
  const Vec3 axisX = dir / len;
  const Vec3 up = IsOne(Abs(axisX.y)) ? Vec3{0,0,-1} : Vec3{0,1,0};
  const Vec3 axisZ = UnitVec(CrossProduct(up, axisX));
  const Vec3 axisY = UnitVec(CrossProduct(axisX, axisZ));

  // use bound of object without parent transform for stretch calc
  const BBox b = ob->bound(&t->base);

  t->base.translate(-b.center());
  t->base.scaleX(len / b.xlength());
  t->base *= {
    axisX.x,  axisX.y,  axisX.z,  0,
    axisY.x,  axisY.y,  axisY.z,  0,
    axisZ.x,  axisZ.y,  axisZ.z,  0,
    center.x, center.y, center.z, 1};
  return 0;
}

static int StretchYFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  // stretch object along its Y axis between 2 points
  Transform* t = findTrans(p);
  Object* ob = dynamic_cast<Object*>(p);
  if (!t || !ob) { return -1; }

  Vec3 p1, p2;
  if (sp.getVec3(n, p1) || sp.getVec3(n, p2) || notDone(sp, n)) { return -1; }

  const Vec3 dir = p2 - p1;
  const Flt len = dir.length();
  if (IsZero(len)) {
    println_err("Invalid stretch length");
    return -1;
  }

  const Vec3 center = (p1+p2) * .5;
  const Vec3 axisY = dir / len;
  const Vec3 side = IsOne(Abs(axisY.x)) ? Vec3{0,-1,0} : Vec3{1,0,0};
  const Vec3 axisZ = UnitVec(CrossProduct(side, axisY));
  const Vec3 axisX = UnitVec(CrossProduct(axisY, axisZ));

  // use bound of object without parent transform for stretch calc
  const BBox b = ob->bound(&t->base);

  t->base.translate(-b.center());
  t->base.scaleY(len / b.ylength());
  t->base *= {
    axisX.x,  axisX.y,  axisX.z,  0,
    axisY.x,  axisY.y,  axisY.z,  0,
    axisZ.x,  axisZ.y,  axisZ.z,  0,
    center.x, center.y, center.z, 1};
  return 0;
}

static int StretchZFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  // stretch object along its Z axis between 2 points
  Transform* t = findTrans(p);
  Object* ob = dynamic_cast<Object*>(p);
  if (!t || !ob) { return -1; }

  Vec3 p1, p2;
  if (sp.getVec3(n, p1) || sp.getVec3(n, p2) || notDone(sp, n)) { return -1; }

  const Vec3 dir = p2 - p1;
  const Flt len = dir.length();
  if (IsZero(len)) {
    println_err("Invalid stretch length");
    return -1;
  }

  const Vec3 center = (p1+p2) * .5;
  const Vec3 axisZ = dir / len;
  const Vec3 up = IsOne(Abs(axisZ.y)) ? Vec3{0,0,-1} : Vec3{0,1,0};
    // FIXME - may need to make 'up' configurable or come up with a better
    //   rule for when axisZ == +/- 1
  const Vec3 axisX = UnitVec(CrossProduct(up, axisZ));
  const Vec3 axisY = UnitVec(CrossProduct(axisZ, axisX));

  // use bound of object without parent transform for stretch calc
  const BBox b = ob->bound(&t->base);

  t->base.translate(-b.center());
  t->base.scaleZ(len / b.zlength());
  t->base *= {
    axisX.x,  axisX.y,  axisX.z,  0,
    axisY.x,  axisY.y,  axisY.z,  0,
    axisZ.x,  axisZ.y,  axisZ.z,  0,
    center.x, center.y, center.z, 1};
  return 0;
}

// scene attributes
static int ApertureFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.aperture) || notDone(sp, n)) { return -1; }
  return 0;
}

static int CoiFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.coi) || notDone(sp, n)) { return -1; }
  return 0;
}

static int EyeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.eye) || notDone(sp, n)) { return -1; }
  return 0;
}

static int FocusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.focus) || notDone(sp, n)) { return -1; }
  return 0;
}

static int FovFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.fov) || notDone(sp, n)) { return -1; }
  return 0;
}

static int JitterFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.jitter) || notDone(sp, n)) { return -1; }
  return 0;
}

static int MaxdepthFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.max_ray_depth) || notDone(sp, n)) { return -1; }
  return 0;
}

static int MinValueFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.min_ray_value) || notDone(sp, n)) { return -1; }
  return 0;
}

static int RegionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.region_min[0])
      || sp.getInt(n, s.region_min[1])
      || sp.getInt(n, s.region_max[0])
      || sp.getInt(n, s.region_max[1])
      || notDone(sp, n)) { return -1; }
  return 0;
}

static int SamplesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  int value;
  if (sp.getInt(n, value) || notDone(sp, n)) { return -1; }

  if (p) {
    return p->setSamples(value);
  } else {
    s.samples = value;
    return 0;
  }
}

static int ShadowBoolFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getBool(n, s.shadow) || notDone(sp, n)) { return -1; }
  return 0;
}

static int SizeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.image_width) || sp.getInt(n, s.image_height)
      || notDone(sp, n)) { return -1; }

  s.region_min[0] = 0;
  s.region_min[1] = 0;
  s.region_max[0] = s.image_width  - 1;
  s.region_max[1] = s.image_height - 1;
  return 0;
}

static int SuperSampleFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.sample_x) || sp.getInt(n, s.sample_y)
      || notDone(sp, n)) { return -1; }
  return 0;
}

static int VupFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.vup) || notDone(sp, n)) { return -1; }
  return 0;
}

// object/light/shader attributes
static int BorderwidthFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setBorderwidth(val);
}

static int CostFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setCost(val);
}

static int DirectionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  if (!lt || sp.getVec3(n, lt->dir) || notDone(sp, n)) { return -1; }
  return 0;
}

static int ExpFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Phong* sh = dynamic_cast<Phong*>(p);
  if (!sh || sp.getFlt(n, sh->exp) || notDone(sp, n)) { return -1; }
  return 0;
}

static int OffsetFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setOffset(val);
}

static int RadiusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setRadius(val);
}

static int RgbFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Vec3 c;
  if (sp.getVec3(n, c) || notDone(sp, n)) { return -1; }

  auto sh = makeShader<ShaderColor>(c.r, c.g, c.b);
  int error = 0;
  if (!p) {
    error = s.addShader(sh, flag);
  } else {
    error = p->addShader(sh, flag);
    if (!error && !dynamic_cast<Shader*>(p)) {
      error = s.addShader(sh, FLAG_INIT_ONLY); // add to scene for init
    }
  }
  return error;
}

static int SectorsFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  int val;
  if (sp.getInt(n, val) || notDone(sp, n)) { return -1; }
  return p->setSectors(val);
}

static int SidesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  int val;
  if (sp.getInt(n, val) || notDone(sp, n)) { return -1; }
  return p->setSides(val);
}

static int SpinFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setSpin(val);
}

static int ValueFn(
   SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setValue(val);
}


// **** Data ****
using KeywordMap = std::map<std::string,ItemFn,std::less<>>;
static std::unique_ptr<KeywordMap> Keywords;

static void initKeywords()
{
  Keywords = std::make_unique<KeywordMap>();
  *Keywords = {
    // keyword      ItemFn
    {"aperture",    ApertureFn},
    {"borderwidth", BorderwidthFn},
    {"coi",         CoiFn},
    {"cost",        CostFn},
    {"dir",         DirectionFn},
    {"direction",   DirectionFn},
    {"exp",         ExpFn},
    {"eye",         EyeFn},
    {"focus",       FocusFn},
    {"fov",         FovFn},
    {"jitter",      JitterFn},
    {"maxdepth",    MaxdepthFn},
    {"minvalue",    MinValueFn},
    {"move",        MoveFn},
    {"move_xbase",  MoveByBBoxSpotFn<BBox::XBASE>},
    {"move_ybase",  MoveByBBoxSpotFn<BBox::YBASE>},
    {"move_zbase",  MoveByBBoxSpotFn<BBox::ZBASE>},
    {"move_center", MoveByBBoxSpotFn<BBox::CENTER>},
    {"move_xtop",   MoveByBBoxSpotFn<BBox::XTOP>},
    {"move_ytop",   MoveByBBoxSpotFn<BBox::YTOP>},
    {"move_ztop",   MoveByBBoxSpotFn<BBox::ZTOP>},
    {"no_parent",   NoParentFn},
    {"offset",      OffsetFn},
    {"radius",      RadiusFn},
    {"region",      RegionFn},
    {"rgb",         RgbFn},
    {"rotate_x",    RotateByAxisFn<Matrix::XAXIS>},
    {"rotate_y",    RotateByAxisFn<Matrix::YAXIS>},
    {"rotate_z",    RotateByAxisFn<Matrix::ZAXIS>},
    {"samples",     SamplesFn},
    {"scale",       ScaleFn},
    {"scale_x",     ScaleAxisFn<Matrix::XAXIS>},
    {"scale_y",     ScaleAxisFn<Matrix::YAXIS>},
    {"scale_z",     ScaleAxisFn<Matrix::ZAXIS>},
    {"scale_xy",    Scale2AxesFn<Matrix::XAXIS, Matrix::YAXIS>},
    {"scale_xz",    Scale2AxesFn<Matrix::XAXIS, Matrix::ZAXIS>},
    {"scale_yz",    Scale2AxesFn<Matrix::YAXIS, Matrix::ZAXIS>},
    {"scale_xyz",   ScaleAllAxesFn},
    {"sectors",     SectorsFn},
    {"shadow",      ShadowBoolFn},
    {"sides",       SidesFn},
    {"supersample", SuperSampleFn},
    {"size",        SizeFn},
    {"spin",        SpinFn},
    {"stretch_x",   StretchXFn},
    {"stretch_y",   StretchYFn},
    {"stretch_z",   StretchZFn},
    {"value",       ValueFn},
    {"vup",         VupFn}
  };
}


REGISTER_OBJECT_KEYWORD(Cone,"cone");
REGISTER_OBJECT_KEYWORD(Cube,"cube");
REGISTER_OBJECT_KEYWORD(Cylinder,"cylinder");
REGISTER_OBJECT_KEYWORD(Difference,"difference");
REGISTER_OBJECT_KEYWORD(Disc,"disc");
REGISTER_OBJECT_KEYWORD(Group,"group");
REGISTER_OBJECT_KEYWORD(Intersection,"intersect");
REGISTER_OBJECT_KEYWORD(Paraboloid,"paraboloid");
REGISTER_OBJECT_KEYWORD(Plane,"plane");
REGISTER_OBJECT_KEYWORD(Sphere,"sphere");
REGISTER_OBJECT_KEYWORD(Torus,"torus");
REGISTER_OBJECT_KEYWORD(Union,"union");
REGISTER_OBJECT_KEYWORD(Prism,"prism");

REGISTER_LIGHT_KEYWORD(PointLight,"light");
REGISTER_LIGHT_KEYWORD(SpotLight,"spotlight");

REGISTER_SHADER_KEYWORD(Phong,"phong");
REGISTER_SHADER_KEYWORD(ShaderSide,"side");
// ambient shaders
REGISTER_SHADER_KEYWORD(Occlusion,"occlusion");
// color shaders
REGISTER_SHADER_KEYWORD(ColorCube,"colorcube");
// pattern shaders
REGISTER_SHADER_KEYWORD(Checkerboard,"checker");
REGISTER_SHADER_KEYWORD(Checkerboard3D,"checker3d");
REGISTER_SHADER_KEYWORD(Pinwheel,"pinwheel");
REGISTER_SHADER_KEYWORD(Ring,"ring");
REGISTER_SHADER_KEYWORD(SquareRing,"squarering");
REGISTER_SHADER_KEYWORD(Stripe,"stripe");
// map shaders
REGISTER_SHADER_KEYWORD(MapGlobalShader,"map_global");
REGISTER_SHADER_KEYWORD(MapConeShader,"map_cone");
REGISTER_SHADER_KEYWORD(MapCubeShader,"map_cube");
REGISTER_SHADER_KEYWORD(MapCylinderShader,"map_cylinder");
REGISTER_SHADER_KEYWORD(MapParaboloidShader,"map_paraboloid");
REGISTER_SHADER_KEYWORD(MapSphereShader,"map_sphere");
// noise shaders
REGISTER_SHADER_KEYWORD(NoiseShader,"noise");

REGISTER_FLAG_KEYWORD(FLAG_AMBIENT,"ambient");
REGISTER_FLAG_KEYWORD(FLAG_BACKGROUND,"background");
REGISTER_FLAG_KEYWORD(FLAG_DEFAULT_LT,"defaultlight");
REGISTER_FLAG_KEYWORD(FLAG_DEFAULT_OBJ,"default");
REGISTER_FLAG_KEYWORD(FLAG_DIFFUSE,"diffuse");
REGISTER_FLAG_KEYWORD(FLAG_SPECULAR,"specular");
REGISTER_FLAG_KEYWORD(FLAG_TRANSMIT,"transmit");
REGISTER_FLAG_KEYWORD(FLAG_BORDER,"border");


// **** Functions ****
static std::string makeKey(std::string_view keyword)
{
  std::string key{keyword};
  for (char& ch : key) { ch = char(std::tolower(ch)); }
  return key;
}

ItemFn FindItemFn(std::string_view keyword)
{
  auto i = Keywords->find(makeKey(keyword));
  return (i != Keywords->end()) ? i->second : nullptr;
}

bool AddItemFn(std::string_view keyword, ItemFn fn)
{
  if (!Keywords) { initKeywords(); }

  const std::string key = makeKey(keyword);
  assert(Keywords->find(key) == Keywords->end());

  (*Keywords)[key] = fn;
  return true;
}
