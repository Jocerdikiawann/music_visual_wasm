#include "screen.h"
#include "visual.h"

ScreenVisualizer sv = {.screenHeight = 0, .screenWidth = 0};

void update_draw_frame_wrapper(void) { update_draw_frame(&sv); }

int main() {
  ChangeDirectory("assets");
#ifdef PLATFORM_ANDROID
  SetTextLineSpacing(30);
#endif
  CreateWindow(&sv);
  create_music();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(update_draw_frame_wrapper, 0, 1);
#else
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
