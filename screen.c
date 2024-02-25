#include "screen.h"

void CreateWindow(ScreenVisualizer *sv) {
#if defined(PLATFORM_WEB)
  int display = GetCurrentMonitor(), fullWidth = GetMonitorWidth(display),
      fullHeight = GetMonitorHeight(display);
  sv->screenHeight = fullHeight - 250;
  sv->screenWidth = fullWidth - 40;
#endif

  InitWindow(sv->screenWidth, sv->screenHeight, sv->title);
}

void DestroyWindow() { CloseWindow(); }
