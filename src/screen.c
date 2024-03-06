#include "screen.h"

void CreateWindow(ScreenVisualizer *sv) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

  int display = GetCurrentMonitor(), fullWidth = GetMonitorWidth(display),
      fullHeight = GetMonitorHeight(display);

#ifdef PLATFORM_WEB
  fullHeight -= 250;
  fullWidth -= 40;
#elif PLATFORM_ANDROID
  fullHeight = 0;
  fullWidth = 0;
#else
  int factor = 70;
  fullWidth = factor * 16;
  fullHeight = factor * 9;
#endif

  InitWindow(fullWidth, fullHeight, sv->title);
}

void DestroyWindow() { CloseWindow(); }
