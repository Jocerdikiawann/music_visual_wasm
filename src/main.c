#include "screen.h"
#include "visual.h"

ScreenVisualizer sv = {.screenHeight = 0, .screenWidth = 0};

void update_draw_frame_wrapper(void) { update_draw_frame(&sv); }

int main() {

  // const char *current_dir = "./storage/";

  // FilePathList fp = LoadDirectoryFilesEx(current_dir, ".ogg", true);

  // for (size_t i = 0; i < fp.count; ++i) {
  //   char *s_dir = fp.paths[i];
  //   TraceLog(LOG_ERROR, TextFormat("Sub Dir %s\n", s_dir));
  // }

  // UnloadDirectoryFiles(fp);
#ifdef PLATFORM_ANDROID
  SetTextLineSpacing(30);
#endif

  CreateWindow(&sv);
  create_music();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(update_draw_frame_wrapper, 0, 1);
#else
  ChangeDirectory("assets");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    update_draw_frame_wrapper();
  }
#endif /* if defined(PLATFORM_WEB) */

  destroy_music();
  free_file_path();
  DestroyWindow();

  return 0;
}
