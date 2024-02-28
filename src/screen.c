#include "screen.h"

void CreateWindow(ScreenVisualizer *sv) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  int factor = 70;
#if defined(PLATFORM_WEB)
  int display = GetCurrentMonitor(), fullWidth = GetMonitorWidth(display),
      fullHeight = GetMonitorHeight(display);
  sv->screenHeight = fullHeight - 250;
  sv->screenWidth = fullWidth - 40;
#endif

  InitWindow(factor * 16, factor * 9, sv->title);
}

void DestroyWindow() { CloseWindow(); }
