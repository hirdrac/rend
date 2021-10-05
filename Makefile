# rend Makefile
# Copyright (C) 2021 Richard Bradley

base_src :=\
  BBox.cc FrameBuffer.cc HitCostInfo.cc Intersect.cc Ray.cc\
  Renderer.cc Roots.cc Scene.cc Stats.cc Transform.cc Timer.cc
object_src :=\
  Object.cc BasicObjects.cc Bound.cc CSG.cc Group.cc Prism.cc
shader_src :=\
  Shader.cc ColorShaders.cc MapShaders.cc PatternShaders.cc Phong.cc
light_src :=\
  Light.cc BasicLights.cc
parser_src :=\
  Parser.cc Tokenizer.cc Keywords.cc


BIN_rend.SRC =\
  $(base_src) $(object_src) $(shader_src) $(light_src) $(parser_src) main.cc


SOURCE_DIR = src
STANDARD = c++17
PACKAGES = readline libpng
OPTIONS = pthread lto modern_c++
WARN_EXTRA = extra-semi fatal-errors conversion cast-align cast-qual
FLAGS = -march=native -ffast-math -fno-plt
#FLAGS_RELEASE = -DNDEBUG

include Makefile.mk

# END
