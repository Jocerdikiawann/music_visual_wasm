#include "screen.h"

void konzolodon_t() {
  InitAudioDevice();
  char *file = (char *)malloc(sizeof(char *) * 200);
  if (file == NULL) {
    printf("Failed Allocate memory at %d\n", __LINE__);
    exit(1);
  }
}
void CreateWindow(ScreenVisualizer *sv) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);

  int factor = 70;
#if defined(PLATFORM_WEB) && defined(PLATFORM_ANDROID)
  int display = GetCurrentMonitor(), fullWidth = GetMonitorWidth(display),
      fullHeight = GetMonitorHeight(display);
  sv->screenHeight = fullHeight - 250;
  sv->screenWidth = fullWidth - 40;
#elif defined(PLATFORM_DESKTOP)
  sv->screenWidth = factor * 16;
  sv->screenHeight = factor * 9;
#endif

  InitWindow(sv->screenWidth, sv->screenHeight, sv->title);
}

void DestroyWindow() { CloseWindow(); }
