//
// main.cc
// Copyright (C) 2024 Richard Bradley
//
// startup for rend
//

#include "Scene.hh"
#include "Parser.hh"
#include "FrameBuffer.hh"
#include "Renderer.hh"
#include "Object.hh"
#include "Light.hh"
#include "Stats.hh"
#include "Print.hh"
#include "PrintList.hh"
#include "CmdLineParser.hh"
#include <sstream>
#include <thread>
#include <chrono>
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

[[nodiscard]] int64_t usecTime()
{
  const auto t = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(
    t.time_since_epoch()).count();
}

[[nodiscard]] constexpr double secDiff(int64_t t0, int64_t t1)
{
  return double(t1 - t0) / 1000000.0;
}

int ShellRender(Renderer& ren, Scene& s, FrameBuffer& fb)
{
  if (s.objects().empty()) {
    println_err("No scene loaded yet");
    return -1;
  }

  const int samples = s.samplesPerPixel();
  println("Rendering ", s.image_width, "x", s.image_height, " image (",
          samples, ((samples == 1) ? " sample" : " samples"), "/pixel)");
  const int64_t t0 = usecTime();

  if (s.init()) {
    println_err("Scene Initialization Failed - can't render");
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
    println_err("Failed to initialize renderer");
    return -1;
  }

  const int64_t t1 = usecTime();
  if (ren.jobs() <= 0) {
    // render on main thread
    JobState js;
    js.init(s);
    for (int y = s.region_min[1]; y <= s.region_max[1]; ++y) {
      print_err("\rScanlines remaining -- ", s.region_max[1] - y, " \b");
      ren.render(js, s.region_min[0], y, s.region_max[0], y);
    }
    ren.setStats(js.stats);
  } else {
    // render on spawned thread(s)
    ren.startJobs();
    int tr, last_tr = -1;
    do {
      tr = ren.waitForJobs(50);
      if (tr != last_tr) {
        print_err("\rTasks remaining -- ", tr, " \b");
        last_tr = tr;
      }
    } while (tr > 0);
    ren.stopJobs();
  }

  const auto t2 = usecTime();
  println("\rTotal Time: ", secDiff(t0,t2), "  (setup ", secDiff(t0,t1),
          ", rendering ", secDiff(t1,t2), ")");
  return 0;
}

int ShellSave(const FrameBuffer& fb, const std::string& file)
{
  if (fb.width() <= 0 || fb.height() <= 0) {
    println_err("No image to save");
    return -1;
  }

  const auto x = file.rfind('.');
  std::string ext;
  if (x != std::string::npos) { ext = file.substr(x+1); }

  if (ext.empty()) { ext = "png"; }
  else if (ext != "bmp" && ext != "png") {
    println_err("Invalid image file extension '", ext,
                "'\n(currently supported: bmp,png)");
    return -1;
  }

  const std::string fn = file.substr(0,x) + "." + ext;
  println("Saving image to '", fn, "'");

  int error = -1;
  if (ext == "bmp") {
    error = fb.saveBMP(fn);
  } else if (ext == "png") {
    error = fb.savePNG(fn);
  }

  if (error) { println_err("Image save failed"); }
  return error;
}

void ShellStats(const Renderer& ren, const Scene& s)
{
  const StatInfo& st = ren.stats();
  const uint64_t object_tried = st.objectsTried();
  const uint64_t object_hit = st.objectsHit();

  const int64_t objs = s.object_count - (s.group_count + s.bound_count);
  const int64_t total_rays = int64_t(st.rays.tried + st.shadow_rays.tried);
  const int64_t dumb_tries = total_rays * objs;
  const int64_t total_tries = int64_t(object_tried + st.bound.tried);

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
  std::unique_ptr<char,decltype(free)*> buffer{readline("REND> "), free};
  if (!buffer) {
    // Control-D pressed
    println();
    return 0;
  }

  add_history(buffer.get());
  std::istringstream input{buffer.get()};

  std::string arg;
  if (!(input >> arg)) {
    println_err("Please enter a command or '?' for help");
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
      println_err("Number of jobs required");
    } else {
      const int j = std::stoi(arg);
      if (j < 0) {
	println_err("Invalid number of jobs '", arg, "'");
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
      println_err("Load requires a file name");
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


int Usage(const char* const* argv)
{
  println("Usage: ", argv[0], " [options] [<scene file> [<image save>]]");
  println("Options:");
  println("  -j [#], --jobs [#]  Use multiple render jobs/threads");
  println("                      (uses ", std::thread::hardware_concurrency(),
          " if option is used without #)");
  println("  -i, --interactive   Start interactive shell");
  println("  -h, --help          Show usage");
  return 0;
}

int ErrorUsage(const char* const* argv)
{
  println_err("Try '", argv[0], " --help' for usage information.");
  return -1;
}


// **** Main Function ****
int main(int argc, char** argv)
{
  println("Rend v0.1 (alpha) - Copyright (C) 2024 Richard Bradley");

  std::string fileLoad, imageSave;
  bool interactive = false;
  int jobs = -1;

  for (CmdLineParser p{argc, argv}; p; ++p) {
    if (p.option()) {
      if (p.option('i',"interactive")) {
        interactive = true;
      } else if (p.option('h',"help")) {
        return Usage(argv);
      } else if (p.option('j',"jobs",jobs)) {
        // jobs value specified
      } else if (p.option('j',"jobs")) {
        jobs = int(std::thread::hardware_concurrency());
      } else {
        println_err("ERROR: Bad option '", p.arg(), "'");
        return ErrorUsage(argv);
      }
    } else if (fileLoad.empty()) {
      p.get(fileLoad);
    } else if (imageSave.empty()) {
      p.get(imageSave);
    } else {
      break; // ignore additional arguments
    }
  }

  if (!interactive && (fileLoad.empty() || imageSave.empty())) {
    println_err("ERROR: Missing arguments");
    return ErrorUsage(argv);
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
