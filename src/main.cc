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
#include "Light.hh"
#include <sstream>
#include <string_view>
#include <thread>
#include <cctype>
#include <readline/readline.h>
#include <readline/history.h>


// **** Functions ****
int ShellInfo(const Scene& s, const FrameBuffer& fb)
{
  if (s.objects().empty()) {
    println("No scene loaded");
    return -1;
  }

  println("    Image size:\t", s.image_width, " x ", s.image_height);
  println("           Fov:\t", s.fov);
  println("       Eye/Coi:\t", s.eye, " / ", s.coi);
  println("    VUP vector:\t", s.vup);
  println(" Max ray depth:\t", s.max_ray_depth);
  println(" Min ray value:\t", s.min_ray_value);

  print("Light List:");
  for (auto& lt : s.lights()) { println("  ", lt->desc()); }
  println();

  float min, max;
  fb.range(min, max);
  println("Color ranges: ", min, " to ", max);
  return 0;
}


int ShellLoad(Scene& s, const std::string& file)
{
  SceneParser parser;
  if (parser.loadFile(file) < 0) {
    return -1;
  }

  println("Loading scene file '", file, "'");
  s.clear();
  if (parser.setupScene(s)) {
    println("Scene loading failed");
    return -1;
  }

  return 0;
}


int ShellRender(Renderer& ren, Scene& s, FrameBuffer& fb)
{
  if (s.objects().empty()) {
    println("No scene loaded yet");
    return -1;
  }

  println("Rendering ", s.image_width, "x", s.image_height,
          " image (", s.samples_x, "x", s.samples_y, " samples)");
  Timer t;
  t.start();

  if (s.init()) {
    println("Scene Initialization Failed - can't render");
    return -1;
  }

#if 0
  // FIXME - output in verbose mode
  if (!s.optObjects().empty()) {
    PrintList(s.optObjects());
  }
#endif

  // Render image
  if (ren.init(&s, &fb)) {
    LOG_ERROR("Failed to initialize renderer");
    return -1;
  }

  auto setupTime = t.elapsedSec();
  if (ren.jobs() <= 0) {
    // render on main thread
    HitCache freeCache;
    for (int y = s.region_min[1]; y <= s.region_max[1]; ++y) {
      print_err("\rscanline -- ", y+1);
      ren.render(s.region_min[0], y, s.region_max[0], y, &freeCache, nullptr);
    }
  } else {
    // render on spawned thread(s)
    ren.startJobs();
    int tr;
    do {
      tr = ren.waitForJobs(50);
      print_err("tasks remaining -- ", tr, "   \r");
    } while (tr > 0);
    ren.stopJobs();
  }

  t.stop();
  auto totalTime = t.elapsedSec();
  println("\rTotal Time: ", totalTime, "  (setup ", setupTime,
          ", rendering ", (totalTime - setupTime), ")");
  return 0;
}

int ShellSave(const FrameBuffer& fb, const std::string& file)
{
  if (fb.width() <= 0 || fb.height() <= 0) {
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
  return fb.saveBMP(fn);
}

void ShellStats(const Renderer& ren, const Scene& s)
{
  const StatInfo& st = ren.stats();
  uint64_t object_tried = st.disc.tried + st.cone.tried + st.cube.tried
    + st.cylinder.tried + st.open_cone.tried + st.open_cylinder.tried
    + st.paraboloid.tried + st.plane.tried + st.sphere.tried + st.torus.tried;

  uint64_t object_hit = st.disc.hit + st.cone.hit + st.cube.hit
    + st.cylinder.hit + st.open_cone.hit + st.open_cylinder.hit
    + st.paraboloid.hit + st.plane.hit + st.sphere.hit + st.torus.hit;

  int64_t objs = s.object_count - (s.group_count + s.bound_count);
  int64_t total_rays = int64_t(st.rays.tried + st.shadow_rays.tried);
  int64_t dumb_tries = total_rays * objs;
  int64_t total_tries = int64_t(object_tried + st.bound.tried);

  println("       Rays Cast  ", st.rays.tried);
  println("        Rays Hit  ", st.rays.hit);
  println("Shadow Rays Cast  ", st.shadow_rays.tried);
  println(" Shadow Rays Hit  ", st.shadow_rays.hit);
  println("     Light Count  ", s.lights().size());
  println("    Shader Count  ", s.shader_count);
  println("    Object Count  ", s.object_count);
  println("     Bound Count  ", s.bound_count);
  println("     Group Count  ", s.group_count);
  println("       CSG Count  ", s.csg_count);
  println("   Objects Tried  ", object_tried);
  println("     Objects Hit  ", object_hit);
  println("    Bounds Tried  ", st.bound.tried);
  println("      Bounds Hit  ", st.bound.hit);
  println();
  println(" Total Rays Cast  ", total_rays);
  println(" Total Hit Tries  ", total_tries);
  println("  Dumb Hit Tries  ", dumb_tries);
  if (dumb_tries > 0) {
    const double percent =
      (double(dumb_tries - total_tries) / double(dumb_tries)) * 100.0;
    println("       Reduction  ", percent, '%');
  }
}

int ShellLoop(Renderer& ren, Scene& s, FrameBuffer& fb)
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
    ShellInfo(s, fb);
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
	ren.setJobs(j);
      }
    }
    break;

  case 'l':
    if (input >> arg) {
      ShellLoad(s, arg);
      lastFile = arg;
    } else if (!lastFile.empty()) {
      ShellLoad(s, lastFile);
    } else {
      println("Load requires a file name");
    }
    break;

  case 'o':
    if (s.objects().empty()) {
      println("No object list");
    } else {
      PrintList(s.objects());
    }
    break;

  case 'q':
    println("Quiting");
    return 0; // quit

  case 'r':
    ShellRender(ren, s, fb);
    break;

  case 's':
    if (input >> arg) {
      ShellSave(fb, arg);
    } else {
      std::string name = lastFile.substr(0, lastFile.rfind('.'));
      if (name.empty()) { name = "out"; }
      ShellSave(fb, name);
    }
    break;

  case 'z':
    ShellStats(ren, s);
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
  println("  -j[#], --jobs [#]  Use multiple render jobs");
  println("                     (uses ", std::thread::hardware_concurrency(),
          " jobs if # is unspecified)");
  println("  -i, --interactive  Start interactive shell");
  println("  -h, --help         Show usage\n");
  return 0;
}


// **** Main Function ****
int main(int argc, char** argv)
{
  println("Rend v0.1 (alpha) - Copyright (C) 2021 Richard Bradley");

  std::string fileLoad, imageSave;
  int jobs = -1;
  bool optionsDone = false;
  bool interactive = false;

  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg[0] == '-' && !optionsDone) {
      if (arg == "--") {
        optionsDone = true;
      } else if (arg == "-i" || arg == "--interactive") {
        interactive = true;
      } else if (arg == "-h" || arg == "--help") {
        return Usage(argv[0]);
      } else if (arg == "-j" || arg == "--jobs") {
        if (i >= (argc-1) || *argv[i+1] == '-') {
          jobs = int(std::thread::hardware_concurrency());
        } else {
          arg = argv[++i];
          if (!std::isdigit(arg[0])) {
            println("Bad job count '", arg, "'\n");
            return Usage(argv[0]);
          }
          jobs = std::atoi(argv[i]);
        }
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

  if (!interactive && (fileLoad.empty() || imageSave.empty())) {
    return Usage(argv[0]);
  }

  Renderer ren;
  if (jobs >= 0) {
    println("Render jobs set to ", jobs);
    ren.setJobs(jobs);
  }

  Scene s;
  FrameBuffer fb;
  if (!fileLoad.empty()) {
    if (ShellLoad(s, fileLoad)) { return -1; }
    if (ShellRender(ren, s, fb)) { return -1; }
  }

  if (!imageSave.empty()) {
    ShellSave(fb, imageSave);
  }

  if (interactive) {
    println("Starting Rend Shell - Enter '?' for help, 'Q' to quit");
    while (ShellLoop(ren, s, fb)) { }
  }

  return 0;
}
