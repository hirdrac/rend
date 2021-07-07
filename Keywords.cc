//
// Keywords.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of keywords module
//

#include "Keywords.hh"
#include "Parser.hh"
#include "BasicLights.hh"
#include "BasicObjects.hh"
#include "BasicShaders.hh"
#include "CSG.hh"
#include "Phong.hh"
#include "Group.hh"
#include "Logger.hh"
#include <map>
#include <cctype>


namespace {
  // **** Helper Functions ****
  Transform* findTrans(SceneItem* p)
  {
    Transform* t = p->trans();
    if (!t) {
      LOG_ERROR("SceneItem ", p->desc(), " cannot be transformed");
    }

    return t;
  }

  int addShader(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n,
                SceneItemFlag flag, Shader* sh)
  {
    int error = 0;
    if (!p) {
      error = s.addShader(sh, flag);
    } else {
      error = p->addShader(sh, flag);
      if (!error && (dynamic_cast<Object*>(p) || dynamic_cast<Light*>(p))) {
        error = s.addShader(sh, flag);  // also add to scene
      }
    }

    if (error) {
      delete sh;
      return error;
    }

    return sp.processList(s, sh, n);
  }

  int addObject(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, Object* ob)
  {
    int error = p ? p->addObject(ob) : s.addObject(ob);
    if (error) {
      delete ob;
      return error;
    }

    return sp.processList(s, ob, n);
  }

  int addLight(SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, Light* lt)
  {
    int error = p ? p->addLight(lt) : s.addLight(lt);
    if (error) {
      delete lt;
      return error;
    }

    return sp.processList(s, lt, n);
  }
}


// **** Item Functions ****
int AirFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sp.processList(s, p, n, AIR);
}

int AmbientFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sp.processList(s, p, n, AMBIENT);
}

int BackgroundFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sp.processList(s, p, n, BACKGROUND);
}

int CheckerboardFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new Checkerboard);
}

int CoiFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getVec3(n, s.coi);
}

int ColorCubeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new ColorCube);
}

int ConeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Cone);
}

int CubeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Cube);
}

int CylinderFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Cylinder);
}

int DefaultLightFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sp.processList(s, p, n, DEFAULT_LT);
}

int DefaultObjectFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sp.processList(s, p, n, DEFAULT_OBJ);
}

int DifferenceFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Difference);
}

int DiffuseFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sp.processList(s, p, n, DIFFUSE);
}

int DirectionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sp.getVec3(n, lt->dir) : -1;
}

int DiscFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Disc);
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

int GlobalFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new ShaderGlobal);
}

int GroupFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Group);
}

int IdentityFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  t->local.setIdentity();
  return 0;
}

int IntersectFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Intersection);
}

int LocalFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new ShaderLocal);
}

int MaxdepthFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sp.getInt(n, s.max_ray_depth);
}

int MergeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Merge);
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

int OpenConeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new OpenCone);
}

int OpenCylinderFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new OpenCylinder);
}

int ParaboloidFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Paraboloid);
}

int PhongFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new Phong);
}

int PlaneFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Plane);
}

int PositionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sp.getVec3(n, lt->pos) : -1;
}

int PointLightFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addLight(sp, s, p, n, new PointLight);
}

int RadiusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  Flt val;
  if (int er = sp.getFlt(n, val); er != 0) { return er; }
  return ob->setRadius(val);
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
  return addShader(sp, s, p, n, flag, new ShaderColor(r, g, b));
}

int RingFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new Ring);
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

int SideFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new ShaderSide);
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

int SolidFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  bool val;
  if (int er = sp.getBool(n, val); er != 0) { return er; }
  return ob->setSolid(val);
}

int SpecularFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sp.processList(s, p, n, SPECULAR);
}

int SphereFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Sphere);
}

int SpotlightFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addLight(sp, s, p, n, new SpotLight);
}

int StripeFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sp, s, p, n, flag, new Stripe);
}

#if 0
int TextureFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string filename;
  sp.getString(n, filename);
  return addShader(sp, s, p, n, flag, new TextureMap);
}
#endif

int TorusFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Torus);
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

int TransmitFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sp.processList(s, p, n, TRANSMIT);
}

int UnionFn(
  SceneParser& sp, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sp, s, p, n, new Union);
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
static const std::map<std::string,ItemFn> KeyWords = {
  // KeyWord        ItemFn
  {"air",           AirFn},
  {"ambient",       AmbientFn},
  {"background",    BackgroundFn},
  {"checker",       CheckerboardFn},
  {"checkerboard",  CheckerboardFn},
  {"coi",           CoiFn},
  {"colorcube",     ColorCubeFn},
  {"cone",          ConeFn},
  {"cube",          CubeFn},
  {"cylinder",      CylinderFn},
  {"default",       DefaultObjectFn},
  {"defaultobject", DefaultObjectFn},
  {"defaultlight",  DefaultLightFn},
  {"difference",    DifferenceFn},
  {"diffuse",       DiffuseFn},
  {"dir",           DirectionFn},
  {"direction",     DirectionFn},
  {"disc",          DiscFn},
  {"exp",           ExpFn},
  {"eye",           EyeFn},
  {"fov",           FovFn},
  {"global",        GlobalFn},
  {"group",         GroupFn},
  {"identity",      IdentityFn},
  {"intersect",     IntersectFn},
  {"light",         PointLightFn},
  {"local",         LocalFn},
  {"maxdepth",      MaxdepthFn},
  {"merge",         MergeFn},
  {"minvalue",      MinValueFn},
  {"move",          MoveFn},
  {"name",          NameFn},
  {"opencone",      OpenConeFn},
  {"opencylinder",  OpenCylinderFn},
  {"paraboloid",    ParaboloidFn},
  {"plane",         PlaneFn},
  {"phong",         PhongFn},
  {"pos",           PositionFn},
  {"position",      PositionFn},
  {"radius",        RadiusFn},
  {"region",        RegionFn},
  {"rgb",           RgbFn},
  {"ring",          RingFn},
  {"rotatex",       RotateXFn},
  {"rotatey",       RotateYFn},
  {"rotatez",       RotateZFn},
  {"rotx",          RotateXFn},
  {"roty",          RotateYFn},
  {"rotz",          RotateZFn},
  {"samples",       SamplesFn},
  {"scale",         ScaleFn},
  {"shadow",        ShadowBoolFn},
  {"side",          SideFn},
  {"size",          SizeFn},
  {"solid",         SolidFn},
  {"specular",      SpecularFn},
  {"sphere",        SphereFn},
  {"spotlight",     SpotlightFn},
  {"stripe",        StripeFn},
  //{"texture",       TextureFn}, FIX!!!
  {"transmit",      TransmitFn},
  {"torus",         TorusFn},
  {"union",         UnionFn},
  {"value",         ValueFn},
  {"vup",           VupFn}
};


// **** Functions ****
ItemFn FindItemFn(const std::string& str)
{
  std::string key = str;
  for (char& ch : key) { ch = char(std::tolower(ch)); }

  auto i = KeyWords.find(key);
  return (i != KeyWords.end()) ? i->second : nullptr;
}
