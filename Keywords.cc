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
#include "BasicShaders.hh"
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
  Transform* t = p->trans();
  if (!t) { println_err("ERROR: ", p->desc(), " cannot be transformed"); }
  return t;
}


// **** Item Functions ****
int CoiFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getVec3(n, s.coi);
}

int DirectionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sp.getVec3(n, lt->dir) : -1;
}

int ExpFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Phong* sh = dynamic_cast<Phong*>(p);
  return sh ? sp.getFlt(n, sh->exp) : -1;
}

int EyeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getVec3(n, s.eye);
}

int FovFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getFlt(n, s.fov);
}

int IdentityFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  t->local.setIdentity();
  return 0;
}

int JitterFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sp.getFlt(n, s.jitter); er != 0) { return er; }
  return 0;
}

int MaxdepthFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getInt(n, s.max_ray_depth);
}

int MinValueFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getFlt(n, s.min_ray_value);
}

int NameFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string buffer;
  if (int er = sp.getString(n, buffer); er != 0) { return er; }
  return buffer.empty() ? -1 : p->setName(buffer);
}

int PositionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sp.getVec3(n, lt->pos) : -1;
}

int RadiusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Flt val;
  if (int er = sp.getFlt(n, val); er != 0) { return er; }
  return p->setRadius(val);
}

int RegionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sp.getInt(n, s.region_min[0]); er != 0) { return er; }
  if (int er = sp.getInt(n, s.region_min[1]); er != 0) { return er; }
  if (int er = sp.getInt(n, s.region_max[0]); er != 0) { return er; }
  if (int er = sp.getInt(n, s.region_max[1]); er != 0) { return er; }
  return 0;
}

int RgbFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Flt r, g, b;
  if (int er = sp.getFlt(n, r); er != 0) { return er; }
  if (int er = sp.getFlt(n, g); er != 0) { return er; }
  if (int er = sp.getFlt(n, b); er != 0) { return er; }

  auto sh = makeShader<ShaderColor>(r, g, b);
  int error = p ? p->addShader(sh, flag) : s.addShader(sh, flag);
  return error ? error : sp.processList(s, sh.get(), n);
}

int RotateXFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sp.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateX(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateYFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sp.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateY(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateZFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sp.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateZ(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int SamplesFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sp.getInt(n, s.samples_x); er != 0) { return er; }
  if (int er = sp.getInt(n, s.samples_y); er != 0) { return er; }
  return 0;
}

int ScaleFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (int er = sp.getVec3(n, v); er != 0) { return er; }
  t->local.scale(v.x, v.y, v.z);
  return 0;
}

int ShadowBoolFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getBool(n, s.shadow);
}

int SizeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sp.getInt(n, s.image_width); er != 0) { return er; }
  if (int er = sp.getInt(n, s.image_height); er != 0) { return er; }

  s.region_max[0] = s.image_width  - 1;
  s.region_max[1] = s.image_height - 1;
  return 0;
}

int MoveFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (int er = sp.getVec3(n, v); er != 0) { return er; }
  t->local.translate(v.x, v.y, v.z);
  return 0;
}

int ValueFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(p);
  return sh ? sp.getFlt(n, sh->value) : -1;
}

int VupFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getVec3(n, s.vup);
}


// **** Data ****
using KeywordMap = std::map<std::string,ItemFn,std::less<>>;
static std::unique_ptr<KeywordMap> Keywords;

static void initKeywords()
{
  Keywords = std::make_unique<KeywordMap>();
  *Keywords = {
    // keyword        ItemFn
    {"coi",           CoiFn},
    {"dir",           DirectionFn},
    {"direction",     DirectionFn},
    {"exp",           ExpFn},
    {"eye",           EyeFn},
    {"fov",           FovFn},
    {"identity",      IdentityFn},
    {"jitter",        JitterFn},
    {"maxdepth",      MaxdepthFn},
    {"minvalue",      MinValueFn},
    {"move",          MoveFn},
    {"name",          NameFn},
    {"pos",           PositionFn},
    {"position",      PositionFn},
    {"radius",        RadiusFn},
    {"region",        RegionFn},
    {"rgb",           RgbFn},
    {"rotatex",       RotateXFn},
    {"rotatey",       RotateYFn},
    {"rotatez",       RotateZFn},
    {"rotx",          RotateXFn},
    {"roty",          RotateYFn},
    {"rotz",          RotateZFn},
    {"samples",       SamplesFn},
    {"scale",         ScaleFn},
    {"shadow",        ShadowBoolFn},
    {"size",          SizeFn},
    {"value",         ValueFn},
    {"vup",           VupFn}
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

REGISTER_SHADER_KEYWORD(Checkerboard,"checker");
REGISTER_SHADER_KEYWORD(ColorCube,"colorcube");
REGISTER_SHADER_KEYWORD(ShaderGlobal,"global");
REGISTER_SHADER_KEYWORD(ShaderLocal,"local");
REGISTER_SHADER_KEYWORD(Phong,"phong");
REGISTER_SHADER_KEYWORD(Ring,"ring");
REGISTER_SHADER_KEYWORD(ShaderSide,"side");
REGISTER_SHADER_KEYWORD(Stripe,"stripe");

REGISTER_FLAG_KEYWORD(FLAG_AIR,"air");
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
