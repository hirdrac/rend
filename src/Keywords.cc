//
// Keywords.cc
// Copyright (C) 2021 Richard Bradley
//

#include "Keywords.hh"
#include "RegisterKeyword.hh"
#include "Parser.hh"
#include "Scene.hh"
#include "BasicLights.hh"
#include "BasicObjects.hh"
#include "PatternShaders.hh"
#include "MapShaders.hh"
#include "CSG.hh"
#include "Phong.hh"
#include "Group.hh"
#include "Logger.hh"
#include "Print.hh"
#include <map>
#include <memory>
#include <cctype>


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
int MoveFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (sp.getVec3(n, v) || notDone(sp, n)) { return -1; }

  t->local.translate(v);
  return 0;
}

int RotateXFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (sp.getFlt(n, angle) || notDone(sp, n)) { return -1; }

  t->local.rotateX(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateYFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (sp.getFlt(n, angle) || notDone(sp, n)) { return -1; }

  t->local.rotateY(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateZFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (sp.getFlt(n, angle) || notDone(sp, n)) { return -1; }

  t->local.rotateZ(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int ScaleFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (sp.getVec3(n, v) || notDone(sp, n)) { return -1; }

  t->local.scale(v);
  return 0;
}

int StretchFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  // TODO: rename to StretchZ, make StretchX,StretchY

  // stretch object along its Z axis between 2 points
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 p1, p2;
  if (sp.getVec3(n, p1) || sp.getVec3(n, p2) || notDone(sp, n)) { return -1; }

  const Vec3 center = (p1+p2) * .5;
  const Vec3 dir = p2 - p1;
  const Flt len = dir.length();
  if (IsZero(len)) {
    println_err("Invalid stretch length");
    return -1;
  }

  const Vec3 axisZ = dir / len;
  const Vec3 up = IsOne(Abs(axisZ.y)) ? Vec3{0,0,-1} : Vec3{0,1,0};
    // FIXME - may need to make 'up' configurable or come up with a better
    //   rule for when axisZ == +/- 1
  const Vec3 axisX = UnitVec(CrossProduct(up, axisZ));
  const Vec3 axisY = UnitVec(CrossProduct(axisZ, axisX));

  t->local.scale(1, 1, len * .5);
  t->local *= {
    axisX.x,  axisX.y,  axisX.z,  0,
    axisY.x,  axisY.y,  axisY.z,  0,
    axisZ.x,  axisZ.y,  axisZ.z,  0,
    center.x, center.y, center.z, 1};
  return 0;
}

// scene attributes
int CoiFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.coi) || notDone(sp, n)) { return -1; }
  return 0;
}

int EyeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.eye) || notDone(sp, n)) { return -1; }
  return 0;
}

int FovFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.fov) || notDone(sp, n)) { return -1; }
  return 0;
}

int JitterFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.jitter) || notDone(sp, n)) { return -1; }
  return 0;
}

int MaxdepthFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.max_ray_depth) || notDone(sp, n)) { return -1; }
  return 0;
}

int MinValueFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getFlt(n, s.min_ray_value) || notDone(sp, n)) { return -1; }
  return 0;
}

int RegionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.region_min[0])
      || sp.getInt(n, s.region_min[1])
      || sp.getInt(n, s.region_max[0])
      || sp.getInt(n, s.region_max[1])
      || notDone(sp, n)) { return -1; }
  return 0;
}

int SamplesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.samples) || notDone(sp, n)) { return -1; }
  return 0;
}

int ShadowBoolFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getBool(n, s.shadow) || notDone(sp, n)) { return -1; }
  return 0;
}

int SizeFn(
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

int SuperSampleFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getInt(n, s.sample_x) || sp.getInt(n, s.sample_y)
      || notDone(sp, n)) { return -1; }
  return 0;
}

int VupFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || sp.getVec3(n, s.vup) || notDone(sp, n)) { return -1; }
  return 0;
}

// object/light/shader attributes
int CostFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setCost(val);
}

int DirectionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  if (!lt || sp.getVec3(n, lt->dir) || notDone(sp, n)) { return -1; }
  return 0;
}

int ExpFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Phong* sh = dynamic_cast<Phong*>(p);
  if (!sh || sp.getFlt(n, sh->exp) || notDone(sp, n)) { return -1; }
  return 0;
}

int PositionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  if (!lt || sp.getVec3(n, lt->pos) || notDone(sp, n)) { return -1; }
  return 0;
}

int RadiusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (!p) { return -1; }

  Flt val;
  if (sp.getFlt(n, val) || notDone(sp, n)) { return -1; }
  return p->setRadius(val);
}

int RgbFn(
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


// **** Data ****
using KeywordMap = std::map<std::string,ItemFn,std::less<>>;
static std::unique_ptr<KeywordMap> Keywords;

static void initKeywords()
{
  Keywords = std::make_unique<KeywordMap>();
  *Keywords = {
    // keyword      ItemFn
    {"coi",         CoiFn},
    {"cost",        CostFn},
    {"dir",         DirectionFn},
    {"direction",   DirectionFn},
    {"exp",         ExpFn},
    {"eye",         EyeFn},
    {"fov",         FovFn},
    {"jitter",      JitterFn},
    {"maxdepth",    MaxdepthFn},
    {"minvalue",    MinValueFn},
    {"move",        MoveFn},
    {"pos",         PositionFn},
    {"position",    PositionFn},
    {"radius",      RadiusFn},
    {"region",      RegionFn},
    {"rgb",         RgbFn},
    {"rotatex",     RotateXFn},
    {"rotatey",     RotateYFn},
    {"rotatez",     RotateZFn},
    {"rotx",        RotateXFn},
    {"roty",        RotateYFn},
    {"rotz",        RotateZFn},
    {"samples",     SamplesFn},
    {"scale",       ScaleFn},
    {"shadow",      ShadowBoolFn},
    {"supersample", SuperSampleFn},
    {"size",        SizeFn},
    {"stretch",     StretchFn},
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
REGISTER_OBJECT_KEYWORD(Merge,"merge");
REGISTER_OBJECT_KEYWORD(OpenCone,"opencone");
REGISTER_OBJECT_KEYWORD(OpenCylinder,"opencylinder");
REGISTER_OBJECT_KEYWORD(Paraboloid,"paraboloid");
REGISTER_OBJECT_KEYWORD(Plane,"plane");
REGISTER_OBJECT_KEYWORD(Sphere,"sphere");
REGISTER_OBJECT_KEYWORD(Torus,"torus");
REGISTER_OBJECT_KEYWORD(Union,"union");

REGISTER_LIGHT_KEYWORD(PointLight,"light");
REGISTER_LIGHT_KEYWORD(SpotLight,"spotlight");

REGISTER_SHADER_KEYWORD(Phong,"phong");
// pattern shaders
REGISTER_SHADER_KEYWORD(Checkerboard,"checker");
REGISTER_SHADER_KEYWORD(ColorCube,"colorcube");
REGISTER_SHADER_KEYWORD(Ring,"ring");
REGISTER_SHADER_KEYWORD(ShaderSide,"side");
REGISTER_SHADER_KEYWORD(Stripe,"stripe");
// map shaders
REGISTER_SHADER_KEYWORD(MapGlobalShader,"map-global");
REGISTER_SHADER_KEYWORD(MapConeShader,"map-cone");
REGISTER_SHADER_KEYWORD(MapCubeShader,"map-cube");
REGISTER_SHADER_KEYWORD(MapCylinderShader,"map-cylinder");
REGISTER_SHADER_KEYWORD(MapParaboloidShader,"map-paraboloid");
REGISTER_SHADER_KEYWORD(MapSphereShader,"map-sphere");

REGISTER_FLAG_KEYWORD(FLAG_AMBIENT,"ambient");
REGISTER_FLAG_KEYWORD(FLAG_BACKGROUND,"background");
REGISTER_FLAG_KEYWORD(FLAG_DEFAULT_LT,"defaultlight");
REGISTER_FLAG_KEYWORD(FLAG_DEFAULT_OBJ,"default");
REGISTER_FLAG_KEYWORD(FLAG_DIFFUSE,"diffuse");
REGISTER_FLAG_KEYWORD(FLAG_SPECULAR,"specular");
REGISTER_FLAG_KEYWORD(FLAG_TRANSMIT,"transmit");


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

  std::string key = makeKey(keyword);
  auto i = Keywords->find(key);
  if (i != Keywords->end()) {
    LOG_ERROR("duplicate keyword '", key, "'");
    return false;
  }

  (*Keywords)[key] = fn;
  return true;
}
