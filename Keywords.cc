//
// Keywords.cc - revision 39 (2019/1/10)
// Copyright(C) 2019 by Richard Bradley
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
#include "Color.hh"
#include "Logger.hh"
#include <map>
#include <cctype>


/**** Functions ****/
Transform* FindTrans(SceneItem* p)
{
  Transform* t = p->trans();
  if (!t) {
    LOG_ERROR("SceneItem " << *p << " cannot be transformed");
  }

  return t;
}

int AddShader(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag,
	      Shader* sh)
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

  return ProcessList(s, sh, n);
}

int AddObject(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag,
	      Object* ob)
{
  int error = p ? p->add(ob, flag) : s.add(ob, flag);
  if (error) {
    delete ob;
    return error;
  }

  return ProcessList(s, ob, n);
}

int AddLight(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag,
	     Light* lt)
{
  int error = p ? p->add(lt, flag) : s.add(lt, flag);
  if (error) {
    delete lt;
    return error;
  }

  return ProcessList(s, lt, n);
}


/**** Item Functions ****/
int AirFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return ProcessList(s, p, n, AIR);
}

int AmbientFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return ProcessList(s, p, n, AMBIENT);
}

int BackgroundFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return ProcessList(s, p, n, BACKGROUND);
}

int CheckerboardFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new Checkerboard);
}

int CoiFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetVec3(n, s.coi, {0,0,0});
}

int ColorCubeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new ColorCube);
}

int ConeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Cone);
}

int CubeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Cube);
}

int CylinderFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Cylinder);
}

int DefaultLightFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return ProcessList(s, p, n, DEFAULT_LT);
}

int DefaultObjectFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p || flag != NO_FLAG) { return -1; }
  return ProcessList(s, p, n, DEFAULT_OBJ);
}

int DifferenceFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Difference);
}

int DiffuseFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return ProcessList(s, p, n, DIFFUSE);
}

int DirectionFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? GetVec3(n, lt->dir, {0,1,0}) : -1;
}

int DiscFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Disc);
}

int EnergyFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return dynamic_cast<Light*>(p) ? ProcessList(s, p, n) : -1;
}

int ExpFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Phong* sh = dynamic_cast<Phong*>(p);
  return sh ? GetFlt(n, sh->exp, 5) : -1;
}

int EyeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetVec3(n, s.eye, {0,0,1});
}

int FovFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetFlt(n, s.fov, 50);
}

int GlobalFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new ShaderGlobal);
}

int GroupFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Group);
}

int IdentityFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  t->local.setIdentity();
  return 0;
}

int IndexFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Primitive* ob = dynamic_cast<Primitive*>(p);
  if (ob) { return GetFlt(n, ob->index, 1.0); }

  return p ? -1 : GetFlt(n, s.index, 1.0);
}

int IntersectFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Intersection);
}

int LocalFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new ShaderLocal);
}

int LocationFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Light* lt = dynamic_cast<Light*>(p);
  return lt ? GetVec3(n, lt->pos, {0,0,0}) : -1;
}

int MaxdepthFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetInt(n, s.max_ray_depth, 64);
}

int MergeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Merge);
}

int MinValueFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetFlt(n, s.min_ray_value, VERY_SMALL);
}

int NameFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string buffer;
  int error = GetString(n, buffer, "");
  if (error) { return error; }
  return p->setName(buffer);
}

int OpenConeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new OpenCone);
}

int OpenCylinderFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new OpenCylinder);
}

int ParaboloidFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Paraboloid);
}

int PhongFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new Phong);
}

int PlaneFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Plane);
}

int PtLightFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddLight(s, p, n, flag, new PointLight);
}

int RadiusFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  Flt val = 0;
  int error = GetFlt(n, val, .5);
  return error || ob->setRadius(val);
}

int RegionFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  int error = 0;
  error = GetInt(n, s.region_min[0], 0);
  error = GetInt(n, s.region_min[1], 0);
  error = GetInt(n, s.region_max[0], s.image_width  - 1);
  error = GetInt(n, s.region_max[1], s.image_height - 1);
  return error;
}

int RgbFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Flt r, g, b;
  int error = 0;
  error = GetFlt(n, r, 0.0);
  error = GetFlt(n, g, r);
  error = GetFlt(n, b, r);
  if (error) { return error; }

  return AddShader(s, p, n, flag, new ShaderColor(r, g, b));
}

int RingFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new Ring);
}

int RotateFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  // Not implemented
  return 0;
}

int RotateXFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  Flt angle;
  int error = GetFlt(n, angle, 0.0);
  t->local.rotateX(angle * MathVal<Flt>::DEG_TO_RAD);
  return error;
}

int RotateYFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  Flt angle;
  int error = GetFlt(n, angle, 0.0);
  t->local.rotateY(angle * MathVal<Flt>::DEG_TO_RAD);
  return error;
}

int RotateZFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  Flt angle;
  int error = GetFlt(n, angle, 0.0);
  t->local.rotateZ(angle * MathVal<Flt>::DEG_TO_RAD);
  return error;
}

int SamplesFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  int error = 0;
  error = GetInt(n, s.samples_x, 1);
  error = GetInt(n, s.samples_y, 1);
  return error;
}

int ScaleFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  int error = GetVec3(n, v, {1,1,1});
  t->local.scale(v.x, v.y, v.z);
  return error;
}

int ShadowBoolFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetBool(n, s.shadow, true);
}

int SideFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new ShaderSide);
}

int SizeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  if (p) { return -1; }

  int error = 0;
  error = GetInt(n, s.image_width,  256);
  error = GetInt(n, s.image_height, s.image_width);
  s.region_max[0] = s.image_width  - 1;
  s.region_max[1] = s.image_height - 1;
  return error;
}

int SolidFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Object* ob = dynamic_cast<Object*>(p);
  if (!ob) { return -1; }

  bool val;
  int error = GetBool(n, val, ob->isSolid());
  return error || ob->setSolid(val);
}

int SpecularFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return ProcessList(s, p, n, SPECULAR);
}

int SphereFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Sphere);
}

int SpotlightFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddLight(s, p, n, flag, new SpotLight);
}

int StripeFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddShader(s, p, n, flag, new Stripe);
}

#if 0
int TextureFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  std::string filename;
  GetString(n, filename, "texture.rle");

  return AddShader(s, p, n, flag, new TextureMap);
}
#endif

int TorusFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Torus);
}

int TranslateFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Transform* t = FindTrans(p);
  if (!t) { return -1; }

  Vec3 v;
  int error = GetVec3(n, v, {0,0,0});
  t->local.translate(v.x, v.y, v.z);
  return error;
}

int TransmitFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return ProcessList(s, p, n, TRANSMIT);
}

int UnionFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return AddObject(s, p, n, flag, new Union);
}

int ValueFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  Shader* sh = dynamic_cast<Shader*>(p);
  return sh ? GetFlt(n, sh->value, 1.0) : -1;
}

int VupFn(Scene& s, SceneItem* p, AstNode* n, SceneItemFlag flag)
{
  return p ? -1 : GetVec3(n, s.vup, {0,1,0});
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
  {"default",       DefaultObjectFn},
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
  {"light",         PtLightFn},
  {"loc",           LocationFn},
  {"local",         LocalFn},
  {"location",      LocationFn},
  {"maxdepth",      MaxdepthFn},
  {"merge",         MergeFn},
  {"minvalue",      MinValueFn},
  {"name",          NameFn},
  {"opencone",      OpenConeFn},
  {"opencylinder",  OpenCylinderFn},
  {"paraboloid",    ParaboloidFn},
  {"plane",         PlaneFn},
  {"phong",         PhongFn},
  {"pos",           LocationFn},
  {"position",      LocationFn},
  {"radius",        RadiusFn},
  {"region",        RegionFn},
  {"rgb",           RgbFn},
  {"ring",          RingFn},
  {"rotate",        RotateFn},
  {"rotatex",       RotateXFn},
  {"rotatey",       RotateYFn},
  {"rotatez",       RotateZFn},
  {"rot",           RotateFn},
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
  {"translate",     TranslateFn},
  {"union",         UnionFn},
  {"value",         ValueFn},
  {"vup",           VupFn}
};


/**** Functions ****/
ItemFn FindItemFn(const std::string& str)
{
  std::string key = str;
  for (char& ch : key) { ch = std::tolower(ch); }

  auto i = KeyWords.find(key);
  return (i != KeyWords.end()) ? i->second : nullptr;
}
