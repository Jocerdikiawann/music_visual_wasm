#include "screen.h"

void CreateWindow(ScreenVisualizer *sv) {
  InitWindow(sv->screenWidth, sv->screenHeight, sv->title);
}

void DestroyWindow() { CloseWindow(); }
