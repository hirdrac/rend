//
// BasicLights.cc
// Copyright (C) 2021 Richard Bradley
//

#include "BasicLights.hh"
#include "Shader.hh"


// **** Sun Class ****
int Sun::init(Scene& s)
{
  _finalDir = _trans.vectorLocalToGlobal(dir, 0);
  return Light::init(s);
}

int Sun::luminate(const Scene& s, const Ray& r, const EvaluatedHit& eh,
                  LightResult& result) const
{
  result.dir = -_finalDir;
  result.distance = VERY_LARGE;
  result.energy = _energy->evaluate(s, r, eh);
  return 0;
}


// **** PointLight Class ****
int PointLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal(pos, 0);
  return Light::init(s);
}

int PointLight::luminate(const Scene& s, const Ray& r, const EvaluatedHit& eh,
                         LightResult& result) const
{
  const Vec3 dir = _finalPos - eh.global_pt;
  const Flt len = dir.length();
  result.dir = dir * (1.0 / len);
  result.distance = len;
  result.energy = _energy->evaluate(s, r, eh);
  return 0;
}


// **** SpotLight Class ****
int SpotLight::init(Scene& s)
{
  _finalPos = _trans.pointLocalToGlobal(pos, 0);
  _finalDir = _trans.vectorLocalToGlobal(dir, 0);
  return Light::init(s);
}

int SpotLight::luminate(const Scene& s, const Ray& r, const EvaluatedHit& eh,
                        LightResult& result) const
{
  // FIX - finish SpotLight::luminate()
  return -1;
}
