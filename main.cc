//
// main.cc
// Copyright (C) 2021 Richard Bradley
//
// startup for rend
//

#include "Scene.hh"
#include "Parse.hh"
#include "FrameBuffer.hh"
#include "Stats.hh"
#include "Timer.hh"
#include "Renderer.hh"
#include "Logger.hh"
#include "Types.hh"
#include <iostream>
#include <cctype>
#include <sstream>
#include <string_view>
#include <readline/readline.h>
#include <readline/history.h>

namespace
{
  /**** Module Globals ****/
  FrameBuffer Fb;
  Scene TheScene;
  Renderer Ren;
}


/**** Functions ****/
int ShellInfo()
{
  if (TheScene.object_list.empty()) {
    std::cout << "No scene loaded yet\n";
    return -1;
  }

  TheScene.info(std::cout);

  float min, max;
  Fb.range(min, max);
  std::cout << "Color ranges: " << min << " to " << max << '\n';
  return 0;
}


int ShellLoad(const std::string& file)
{
  std::cout << "Parsing file '" << file << "'\n";
  SceneDesc sf;
  if (sf.parseFile(file) < 0) {
    std::cout << "Error in loading file\n";
    return -1;
  }

  std::cout << "Generating Scene\n";
  TheScene.clear();
  if (TheScene.generate(sf)) {
    std::cout << "Error in scene generation\n";
    return -1;
  }

  return 0;
}


int ShellRender()
{
  if (TheScene.object_list.empty()) {
    std::cout << "No scene loaded yet\n";
    return -1;
  }

  Fb.clear();
  Timer t;
  t.start();

  if (TheScene.init()) {
    std::cout << "Scene Initialization Failed - can't render\n";
    return -1;
  }

  // Render image
  std::cout << "Rendering (" << TheScene.samples_x << " x "
	    << TheScene.samples_y << " Samples)\n";

  if (Ren.init(&TheScene, &Fb)) {
    LOG_ERROR("Failed to initialize renderer");
    return -1;
  }

  if (Ren.jobs() <= 0) {
    // single thread
    SList<HitInfo> freeCache;
    for (int y = TheScene.region_min[1]; y <= TheScene.region_max[1]; ++y) {
      std::cerr << "\rscanline -- " << y+1;
      Ren.render(TheScene.region_min[0], y, TheScene.region_max[0], y,
		 &freeCache, nullptr);
    }
  } else {
    int j = Ren.jobs();
    Ren.startJobs();
    while (1) {
      int tr = Ren.waitForJobs(50);
      std::cerr << "Jobs: " << j << "   Tasks: " << tr << "   \r";
      if (tr <= 0) { break; }
    }
    Ren.stopJobs();
  }

  t.stop();
  std::cerr << "\rRendering Complete     \n";
  std::cout << "Rendering Time: " << t.elapsedSec() << '\n';
  return 0;
}

int ShellSave(const std::string& file)
{
  std::cout << "Saving image to '" << file << "'\n";
  return Fb.saveBMP(file);
}

int ShellLoop()
{
  std::cout << std::endl;
  char* buffer = readline("REND> ");
  if (!buffer) {
    // Control-D pressed
    std::cout << std::endl;
    return 0;
  }

  add_history(buffer);
  std::istringstream input(buffer);
  free(buffer);

  std::string arg;
  if (!(input >> arg)) {
    std::cerr << "please enter a command or '?' for help\n";
    return -1; // error
  }

  switch (tolower(arg[0])) {
  case '?':
    std::cout << "I        - Info on scene\n"
	      << "J <num>  - Set number of render jobs(threads)\n"
	      << "L <file> - Load scene file\n"
	      << "O        - Show objects\n"
	      << "R        - Render scene\n"
	      << "S <file> - Save image to file\n"
	      << "Z        - Show render stats\n";
    break;

  case 'i':
    ShellInfo();
    break;

  case 'j':
    if (!(input >> arg)) {
      std::cout << "Number of jobs required\n";
    } else {
      int j = std::atoi(arg.c_str());
      if (j < 0) {
	std::cout << "Invalid number of jobs '" << arg << "'\n";
      } else {
	std::cout << "Jobs set to " << j << '\n';
	Ren.setJobs(j);
      }
    }
    break;

  case 'l':
    if (input >> arg) {
      ShellLoad(arg);
    } else {
      std::cout << "Load requires a file name\n";
    }
    break;

  case 'o':
    if (TheScene.object_list.head()) {
      PrintList(std::cout, TheScene.object_list.head());
    } else {
      std::cout << "No object list\n";
    }
    break;

  case 'q':
    std::cout << "Quiting\n";
    return 0; // quit

  case 'r':
    ShellRender();
    break;

  case 's':
    if (input >> arg) {
      ShellSave(arg);
    } else {
      ShellSave("out.bmp");
    }
    break;

  case 'z':
    Ren.stats().print(TheScene, std::cout);
    break;

  default:
    std::cout << "Unknown command '" << arg << "' - enter '?' for help\n";
    return -1; // error
  }

  return 1; // don't quit
}

int Usage(int argc, char** argv)
{
  std::cout << "Usage: " << argv[0] << " [options] [<scene file> [<image save>]]\n"
            << "Options:\n"
            << "  -j#, --jobs #   Number of render jobs\n"
            << "  -h, --help      Show usage\n\n";
  return 0;
}


/**** Main Function ****/
int main(int argc, char** argv)
{
  std::cout << "Rend v0.1 (alpha) - Copyright (C) 2021 Richard Bradley\n\n";

  std::string fileLoad, imageSave;
  int jobs = -1;
  bool optionsDone = false;

  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg[0] == '-' && !optionsDone) {
      if (arg == "--") {
        optionsDone = true;
      } else if (arg == "-h" || arg == "--help") {
        return Usage(argc, argv);
      } else if (arg == "-j" || arg == "--jobs") {
        if (i >= (argc-1)) {
          std::cout << "Missing job count\n\n";
          return Usage(argc, argv);
        }
        arg = argv[++i];
        if (!std::isdigit(arg[0])) {
          std::cout << "Bad job count '" << arg << "'\n\n";
          return Usage(argc, argv);
        }
        jobs = std::atoi(argv[i]);
      } else if (arg.size() > 2 && arg.substr(0,2) == "-j" && std::isdigit(arg[2])) {
        jobs = std::atoi(argv[i] + 2);
      } else if (arg.size() > 7 && arg.substr(0,7) == "--jobs=" && std::isdigit(arg[7])) {
        jobs = std::atoi(argv[i] + 7);
      } else {
        std::cout << "Bad option '" << arg << "'\n\n";
        return Usage(argc, argv);
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
    std::cout << "Render jobs set to " << jobs << "\n\n";
    Ren.setJobs(jobs);
  }

  if (!fileLoad.empty()) {
    if (ShellLoad(fileLoad)) { return -1; }
    if (ShellRender()) { return -1; }
  }

  if (!imageSave.empty()) {
    return ShellSave(imageSave);
  }

  std::cout << "\nStarting Rend Shell - Enter '?' for help, 'Q' to quit\n";
  while (ShellLoop()) { }
  return 0;
}
