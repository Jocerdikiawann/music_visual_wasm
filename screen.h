#ifndef SCREEN_H
#define SCREEN_H

#include "ext.h"

typedef struct ScreenVisualizer {
  int screenWidth, screenHeight;
  char *title;
} ScreenVisualizer;

void CreateWindow(ScreenVisualizer *sv);
void FullScreenWindow(ScreenVisualizer *sv);
void DestroyWindow();
#endif // !SCREEN_H
