#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define BUFFER_SIZE 16384
#define RAW_BUCKET_COUNT 200
#define OUTPUT_BUCKET_COUNT 4
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef double _Complex cplx;

typedef struct MusicVisualizer {
  Music music;
  float volume, timeplayed;
  unsigned int sampleRate, sampleBufferSize;
  float *windowCache;
  cplx samples;
  float maxSampleIndex;
  double rawBucketMultiplier;
  int rawBucketPerOutput;
  int frameNumber;
} MusicVisualizer;

void CreateMusic(MusicVisualizer *mv);
void ConstructWindows(MusicVisualizer *mv);

void UpdateDrawFrame(MusicVisualizer *mv, ScreenVisualizer *sv);
void DestroyMusic(MusicVisualizer *mv);

#endif // !VISUAL_H
