#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define N 256

typedef struct MusicVisualizer {
  Music music;
  float volume, timeplayed;
} MusicVisualizer;

void CreateMusic(MusicVisualizer *mv);
void ConstructWindows(MusicVisualizer *mv);
void CollectSamples(MusicVisualizer *mv);
void UpdateDrawFrame(MusicVisualizer *mv, ScreenVisualizer *sv);
void DestroyMusic(MusicVisualizer *mv);

#endif // !VISUAL_H
