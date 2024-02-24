#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define N (1 << 13)

typedef struct MusicVisualizer {
  Music music;
  float volume, timeplayed, frameTime, smoothness;
} MusicVisualizer;

void CreateMusic();
void UpdateDrawFrame(ScreenVisualizer *sv);
void DestroyMusic();

#endif // !VISUAL_H
