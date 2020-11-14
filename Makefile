# rend Makefile - revision 17 (2019/1/23)
# Copyright(C) 2019 by Richard Bradley

BIN_rend.SRC =\
  BasicLights.cc BasicObjects.cc BasicShaders.cc Bound.cc Color.cc CSG.cc\
  FrameBuffer.cc Group.cc HitCostInfo.cc Intersect.cc Keywords.cc Light.cc\
  Logger.cc main.cc Object.cc Parse.cc Phong.cc Ray.cc Renderer.cc Roots.cc\
  Scene.cc Shader.cc Shadow.cc Stats.cc Transform.cc Timer.cc

STANDARD = c++17
PACKAGES = readline
OPTIONS = pthread lto modern_c++
#LIBS = -l:libreadline.a -ltinfo

include Makefile.mk

# END
