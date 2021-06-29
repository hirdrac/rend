//
// Keywords.cc
// Copyright (C) 2021 Richard Bradley
//
// Implementation of keywords module
//

#include "Keywords.hh"
#include "Parse.hh"
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
  /**** Helper Functions ****/
  Transform* findTrans(SceneItem* p)
  {
    Transform* t = p->trans();
    if (!t) {
      LOG_ERROR("SceneItem ", p->desc(0), " cannot be transformed");
    }

    return t;
  }

  int addShader(SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n,
                SceneItemFlag flag, Shader* sh)
  {
    int error = 0;
    if (!p) {
      error = s.add(sh, flag);
    } else {
      error = p->add(sh, flag);
      if (!error && (dynamic_cast<Object*>(p) || dynamic_cast<Light*>(p))) {
        error = s.add(sh, flag);  // also add to scene
      }
    }

    if (error) {
      delete sh;
      return error;
    }

    return sd.processList(s, sh, n);
  }

  int addObject(SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n,
                SceneItemFlag flag, Object* ob)
  {
    int error = p ? p->add(ob, flag) : s.add(ob, flag);
    if (error) {
      delete ob;
      return error;
    }

    return sd.processList(s, ob, n);
  }

  int addLight(SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n,
               SceneItemFlag flag, Light* lt)
  {
    int error = p ? p->add(lt, flag) : s.add(lt, flag);
    if (error) {
      delete lt;
      return error;
    }

    return sd.processList(s, lt, n);
  }
}


/**** Item Functions ****/
int AirFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sd.processList(s, p, n, AIR);
}

int AmbientFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sd.processList(s, p, n, AMBIENT);
}

int BackgroundFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sd.processList(s, p, n, BACKGROUND);
}

int CheckerboardFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new Checkerboard);
}

int CoiFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getVec3(n, s.coi);
}

int ColorCubeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new ColorCube);
}

int ConeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Cone);
}

int CubeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Cube);
}

int CylinderFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Cylinder);
}

int DefaultLightFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sd.processList(s, p, n, DEFAULT_LT);
}

int DefaultObjectFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return sd.processList(s, p, n, DEFAULT_OBJ);
}

int DifferenceFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Difference);
}

int DiffuseFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sd.processList(s, p, n, DIFFUSE);
}

int DirectionFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sd.getVec3(n, lt->dir) : -1;
}

int DiscFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Disc);
}

int EnergyFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return dynamic_cast<Light*>(p) ? sd.processList(s, p, n) : -1;
}

int ExpFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Phong* sh = dynamic_cast<Phong*>(p);
  return sh ? sd.getFlt(n, sh->exp) : -1;
}

int EyeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getVec3(n, s.eye);
}

int FovFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getFlt(n, s.fov);
}

int GlobalFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new ShaderGlobal);
}

int GroupFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Group);
}

int IdentityFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  t->local.setIdentity();
  return 0;
}

int IndexFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Primitive* ob = dynamic_cast<Primitive*>(p);
  if (ob) { return sd.getFlt(n, ob->index); }

  return p ? -1 : sd.getFlt(n, s.index);
}

int IntersectFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Intersection);
}

int LocalFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new ShaderLocal);
}

int MaxdepthFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getInt(n, s.max_ray_depth);
}

int MergeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Merge);
}

int MinValueFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getFlt(n, s.min_ray_value);
}

int NameFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string buffer;
  if (int er = sd.getString(n, buffer); er != 0) { return er; }
  return p->setName(buffer);
}

int OpenConeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new OpenCone);
}

int OpenCylinderFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new OpenCylinder);
}

int ParaboloidFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Paraboloid);
}

int PhongFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new Phong);
}

int PlaneFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Plane);
}

int PositionFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? sd.getVec3(n, lt->pos) : -1;
}

int PointLightFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addLight(sd, s, p, n, flag, new PointLight);
}

int RadiusFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  Flt val;
  if (int er = sd.getFlt(n, val); er != 0) { return er; }
  return ob->setRadius(val);
}

int RegionFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sd.getInt(n, s.region_min[0]); er != 0) { return er; }
  if (int er = sd.getInt(n, s.region_min[1]); er != 0) { return er; }
  if (int er = sd.getInt(n, s.region_max[0]); er != 0) { return er; }
  if (int er = sd.getInt(n, s.region_max[1]); er != 0) { return er; }
  return 0;
}

int RgbFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Flt r, g, b;
  if (int er = sd.getFlt(n, r); er != 0) { return er; }
  if (int er = sd.getFlt(n, g); er != 0) { return er; }
  if (int er = sd.getFlt(n, b); er != 0) { return er; }
  return addShader(sd, s, p, n, flag, new ShaderColor(r, g, b));
}

int RingFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new Ring);
}

int RotateXFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sd.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateX(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateYFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sd.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateY(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int RotateZFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Flt angle;
  if (int er = sd.getFlt(n, angle); er != 0) { return er; }
  t->local.rotateZ(angle * math::DEG_TO_RAD<Flt>);
  return 0;
}

int SamplesFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sd.getInt(n, s.samples_x); er != 0) { return er; }
  if (int er = sd.getInt(n, s.samples_y); er != 0) { return er; }
  return 0;
}

int ScaleFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (int er = sd.getVec3(n, v); er != 0) { return er; }
  t->local.scale(v.x, v.y, v.z);
  return 0;
}

int ShadowBoolFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getBool(n, s.shadow);
}

int SideFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new ShaderSide);
}

int SizeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  if (int er = sd.getInt(n, s.image_width); er != 0) { return er; }
  if (int er = sd.getInt(n, s.image_height); er != 0) { return er; }

  s.region_max[0] = s.image_width  - 1;
  s.region_max[1] = s.image_height - 1;
  return 0;
}

int SolidFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  bool val;
  if (int er = sd.getBool(n, val); er != 0) { return er; }
  return ob->setSolid(val);
}

int SpecularFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sd.processList(s, p, n, SPECULAR);
}

int SphereFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Sphere);
}

int SpotlightFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addLight(sd, s, p, n, flag, new SpotLight);
}

int StripeFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addShader(sd, s, p, n, flag, new Stripe);
}

#if 0
int TextureFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string filename;
  sd.getString(n, filename);
  return addShader(sd, s, p, n, flag, new TextureMap);
}
#endif

int TorusFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Torus);
}

int MoveFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = findTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  if (int er = sd.getVec3(n, v); er != 0) { return er; }
  t->local.translate(v.x, v.y, v.z);
  return 0;
}

int TransmitFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return sd.processList(s, p, n, TRANSMIT);
}

int UnionFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return addObject(sd, s, p, n, flag, new Union);
}

int ValueFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(p);
  return sh ? sd.getFlt(n, sh->value) : -1;
}

int VupFn(
  SceneDesc& sd, Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : sd.getVec3(n, s.vup);
}


/**** Data ****/
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
  {"energy",        EnergyFn},
  {"exp",           ExpFn},
  {"eye",           EyeFn},
  {"fov",           FovFn},
  {"global",        GlobalFn},
  {"group",         GroupFn},
  {"identity",      IdentityFn},
  {"index",         IndexFn},
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


/**** Functions ****/
ItemFn FindItemFn(const std::string& str)
{
  std::string key = str;
  for (char& ch : key) { ch = char(std::tolower(ch)); }

  auto i = KeyWords.find(key);
  return (i != KeyWords.end()) ? i->second : nullptr;
}
