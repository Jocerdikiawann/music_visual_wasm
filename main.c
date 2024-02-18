#include "screen.h"
#include "visual.h"

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif /* ifdef  defined(PLATFORM_WEB) */

ScreenVisualizer sv = {.screenHeight = 450, .screenWidth = 800};
MusicVisualizer mv = {
    .volume = 0.5f,
    .timeplayed = 0.0f,
};

void UpdateDrawFrameWrapper(void) { UpdateDrawFrame(&mv, &sv); }

int main() {
  CreateWindow(&sv);
  CreateMusic(&mv);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrameWrapper, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateDrawFrameWrapper();
  }
#endif /* if defined(PLATFORM_WEB) */

  DestroyMusic(&mv);
  DestroyWindow();

  return 0;
}
