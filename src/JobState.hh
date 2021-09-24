//
// JobState.hh
// Copyright (C) 2021 Richard Bradley
//
// Job (thread) specific data used in rendering
//

#pragma once
#include "Intersect.hh"
#include "Stats.hh"
#include "RandomGen.hh"


struct JobState
{
  HitCache cache;
  StatInfo stats;
  RandomGen rnd;
};
