#include "screen.h"

void CreateWindow(ScreenVisualizer *sv) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

  int factor = 70;
#if defined(PLATFORM_WEB)
  int display = GetCurrentMonitor(), fullWidth = GetMonitorWidth(display),
      fullHeight = GetMonitorHeight(display);
  sv->screenHeight = fullHeight - 250;
  sv->screenWidth = fullWidth - 40;
#elif defined(PLATFORM_DESKTOP)
  sv->screenWidth = factor * 16;
  sv->screenHeight = factor * 9;
#elif defined(PLATFORM_ANROID)
  sv->screenHeight = 800;
  sv->screenWidth = 450;
#endif

  InitWindow(sv->screenWidth, sv->screenHeight, sv->title);
}

void DestroyWindow() { CloseWindow(); }
