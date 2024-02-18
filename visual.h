#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"

typedef struct MusicVisualizer {
  Music music;
  float volume, timeplayed;
} MusicVisualizer;

void CreateMusic(MusicVisualizer *mv);

void UpdateDrawFrame(MusicVisualizer *mv, ScreenVisualizer *sv);
void DestroyMusic(MusicVisualizer *mv);

#endif // !VISUAL_H
