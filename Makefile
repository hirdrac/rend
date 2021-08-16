# rend Makefile
# Copyright (C) 2021 Richard Bradley

raytracer_src :=\
  BasicLights.cc BasicObjects.cc BasicShaders.cc BBox.cc Bound.cc CSG.cc\
  FrameBuffer.cc Group.cc HitCostInfo.cc Intersect.cc Light.cc Logger.cc\
  Object.cc Phong.cc Ray.cc Renderer.cc Roots.cc Scene.cc Shader.cc\
  Stats.cc Transform.cc Timer.cc
parser_src :=\
  Parser.cc Tokenizer.cc Keywords.cc

BIN_rend.SRC = $(raytracer_src) $(parser_src) main.cc


SOURCE_DIR = src
STANDARD = c++17
PACKAGES = readline
OPTIONS = pthread lto modern_c++
WARN_EXTRA = extra-semi fatal-errors conversion cast-align cast-qual
FLAGS = -march=native -ffast-math
#FLAGS_RELEASE = -DNDEBUG

include Makefile.mk

# END
