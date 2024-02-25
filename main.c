#include "screen.h"
#include "visual.h"

ScreenVisualizer sv = {.screenHeight = 600, .screenWidth = 800};

void UpdateDrawFrameWrapper(void) { UpdateDrawFrame(&sv); }

int main() {
  CreateWindow(&sv);
  CreateMusic();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrameWrapper, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateDrawFrameWrapper();
  }
#endif /* if defined(PLATFORM_WEB) */

  DestroyMusic();
  DestroyWindow();

  return 0;
}
