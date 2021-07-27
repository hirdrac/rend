//
// main.cc
// Copyright (C) 2021 Richard Bradley
//
// startup for rend
//

#include "Scene.hh"
#include "Parser.hh"
#include "FrameBuffer.hh"
#include "Stats.hh"
#include "Timer.hh"
#include "Renderer.hh"
#include "Logger.hh"
#include "Types.hh"
#include "Print.hh"
#include "PrintList.hh"
#include "Object.hh"
#include <cctype>
#include <sstream>
#include <string_view>
#include <readline/readline.h>
#include <readline/history.h>

namespace
{
  // **** Module Globals ****
  FrameBuffer Fb;
  Scene TheScene;
  Renderer Ren;
}


// **** Functions ****
int ShellInfo()
{
  if (TheScene.objects().empty()) {
    println("No scene loaded");
    return -1;
  }

  TheScene.info(std::cout);

  float min, max;
  Fb.range(min, max);
  println("Color ranges: ", min, " to ", max);
  return 0;
}


int ShellLoad(const std::string& file)
{
  SceneParser parser;
  if (parser.loadFile(file) < 0) {
    return -1;
  }

  println("Loading scene file '", file, "'");
  TheScene.clear();
  if (parser.setupScene(TheScene)) {
    println("Scene loading failed");
    return -1;
  }

  return 0;
}


int ShellRender()
{
  if (TheScene.objects().empty()) {
    println("No scene loaded yet");
    return -1;
  }

  Fb.clear();
  Timer t;
  t.start();

  if (TheScene.init()) {
    println("Scene Initialization Failed - can't render");
    return -1;
  }

  if (TheScene.bound()) {
    // TODO - limit output to verbose mode
    PrintList(TheScene.bound()->children());
  }

  // Render image
  println("Rendering (", TheScene.samples_x, " x ",
          TheScene.samples_y, " Samples)");

  if (Ren.init(&TheScene, &Fb)) {
    LOG_ERROR("Failed to initialize renderer");
    return -1;
  }

  if (Ren.jobs() <= 0) {
    // single thread
    SList<HitInfo> freeCache;
    for (int y = TheScene.region_min[1]; y <= TheScene.region_max[1]; ++y) {
      print_err("\rscanline -- ", y+1);
      Ren.render(TheScene.region_min[0], y, TheScene.region_max[0], y,
		 &freeCache, nullptr);
    }
  } else {
    int j = Ren.jobs();
    Ren.startJobs();
    while (1) {
      int tr = Ren.waitForJobs(50);
      print_err("Jobs: ", j, "   Tasks: ", tr, "   \r");
      if (tr <= 0) { break; }
    }
    Ren.stopJobs();
  }

  t.stop();
  println_err("\rRendering Complete     ");
  println("Rendering Time: ", t.elapsedSec());
  return 0;
}

int ShellSave(const std::string& file)
{
  if (Fb.width() <= 0 || Fb.height() <= 0) {
    println("No image to save");
    return -1;
  }

  auto x = file.rfind('.');
  std::string ext;
  if (x != std::string::npos) { ext = file.substr(x); }

  if (ext.empty()) { ext = ".bmp"; }
  else if (ext != ".bmp") {
    println("Invalid image file extension '", ext.substr(1), "'");
    println("(only 'bmp' currently supported)");
    return -1;
  }

  std::string fn = file.substr(0,x) + ext;
  println("Saving image to '", fn, "'");
  return Fb.saveBMP(fn);
}

int ShellLoop()
{
  static std::string lastFile;

  println();
  char* buffer = readline("REND> ");
  if (!buffer) {
    // Control-D pressed
    println();
    return 0;
  }

  add_history(buffer);
  std::istringstream input(buffer);
  free(buffer);

  std::string arg;
  if (!(input >> arg)) {
    println_err("please enter a command or '?' for help");
    return -1; // error
  }

  switch (tolower(arg[0])) {
  case '?':
    println("I        - Info on scene");
    println("J <num>  - Set number of render jobs(threads)");
    println("L <file> - Load scene file");
    println("O        - Show objects");
    println("R        - Render scene");
    println("S <file> - Save image to file");
    println("Z        - Show render stats");
    break;

  case 'i':
    ShellInfo();
    break;

  case 'j':
    if (!(input >> arg)) {
      println("Number of jobs required");
    } else {
      int j = std::stoi(arg);
      if (j < 0) {
	println("Invalid number of jobs '", arg, "'");
      } else {
	println("Jobs set to ", j);
	Ren.setJobs(j);
      }
    }
    break;

  case 'l':
    if (input >> arg) {
      ShellLoad(arg);
      lastFile = arg;
    } else if (!lastFile.empty()) {
      ShellLoad(lastFile);
    } else {
      println("Load requires a file name");
    }
    break;

  case 'o':
    if (TheScene.objects().empty()) {
      println("No object list");
    } else {
      PrintList(TheScene.objects());
    }
    break;

  case 'q':
    println("Quiting");
    return 0; // quit

  case 'r':
    ShellRender();
    break;

  case 's':
    if (input >> arg) {
      ShellSave(arg);
    } else {
      std::string name = lastFile.substr(0, lastFile.rfind('.'));
      if (name.empty()) { name = "out"; }
      ShellSave(name);
    }
    break;

  case 'z':
    Ren.stats().print(std::cout);
    break;

  default:
    println("Unknown command '", arg, "' - enter '?' for help");
    return -1; // error
  }

  return 1; // don't quit
}

int Usage(const char* argv0)
{
  println("Usage: ", argv0, " [options] [<scene file> [<image save>]]");
  println("Options:");
  println("  -j#, --jobs #   Number of render jobs");
  println("  -h, --help      Show usage\n");
  return 0;
}


// **** Main Function ****
int main(int argc, char** argv)
{
  println("Rend v0.1 (alpha) - Copyright (C) 2021 Richard Bradley");

  std::string fileLoad, imageSave;
  int jobs = -1;
  bool optionsDone = false;

  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg[0] == '-' && !optionsDone) {
      if (arg == "--") {
        optionsDone = true;
      } else if (arg == "-h" || arg == "--help") {
        return Usage(argv[0]);
      } else if (arg == "-j" || arg == "--jobs") {
        if (i >= (argc-1)) {
          println("Missing job count\n");
          return Usage(argv[0]);
        }
        arg = argv[++i];
        if (!std::isdigit(arg[0])) {
          println("Bad job count '", arg, "'\n");
          return Usage(argv[0]);
        }
        jobs = std::atoi(argv[i]);
      } else if (arg.size() > 2 && arg.substr(0,2) == "-j" && std::isdigit(arg[2])) {
        jobs = std::atoi(argv[i] + 2);
      } else if (arg.size() > 7 && arg.substr(0,7) == "--jobs=" && std::isdigit(arg[7])) {
        jobs = std::atoi(argv[i] + 7);
      } else {
        println("Bad option '", arg, "'\n");
        return Usage(argv[0]);
      }
    } else if (fileLoad.empty()) {
      fileLoad = arg;
    } else if (imageSave.empty()) {
      imageSave = arg;
    } else {
      break;
    }
  }

  if (jobs >= 0) {
    println("Render jobs set to ", jobs);
    Ren.setJobs(jobs);
  }

  if (!fileLoad.empty()) {
    if (ShellLoad(fileLoad)) { return -1; }
    if (ShellRender()) { return -1; }
  }

  if (!imageSave.empty()) {
    return ShellSave(imageSave);
  }

  println("Starting Rend Shell - Enter '?' for help, 'Q' to quit");
  while (ShellLoop()) { }
  return 0;
}
