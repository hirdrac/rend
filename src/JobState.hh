//
// JobState.hh
// Copyright (C) 2022 Richard Bradley
//
// Job (thread) specific data used in rendering
//

#pragma once
#include "HitInfo.hh"
#include "Stats.hh"
#include "RandomGen.hh"


struct JobState
{
  HitCache cache;
  StatInfo stats;
  RandomGen rnd;
};
