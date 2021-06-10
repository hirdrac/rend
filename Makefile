# rend Makefile
# Copyright (C) 2021 Richard Bradley

BIN_rend.SRC =\
  BasicLights.cc BasicObjects.cc BasicShaders.cc Bound.cc Color.cc CSG.cc\
  FrameBuffer.cc Group.cc HitCostInfo.cc Intersect.cc Keywords.cc Light.cc\
  Logger.cc main.cc Object.cc Parse.cc Phong.cc Ray.cc Renderer.cc Roots.cc\
  Scene.cc Shader.cc Shadow.cc Stats.cc Transform.cc Timer.cc

STANDARD = c++17
PACKAGES = readline
OPTIONS = pthread lto modern_c++
WARN_EXTRA = extra-semi fatal-errors conversion cast-align cast-qual


include Makefile.mk

# END
