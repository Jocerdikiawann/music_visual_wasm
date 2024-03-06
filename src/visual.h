#ifndef VISUAL_H
#define VISUAL_H

#ifdef PLATFORM_ANDROID
#include "complex_.h"
#else
#include <complex.h>
#endif
#include "ext.h"
#include "screen.h"

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define BUFFER_SIZE (1 << 13)
#define MAX_FILEPATH_RECORDED 100

// MATH DEFINE
#ifdef PLATFORM_ANDROID
#define Cplx complex_t
#define ccabsf(z) ccabs(z)
#define cfromreal(re) cvalue(re, 0)
#define cfromimag(im) cvalue(0, im)
#define cmul(a, b) cmultiply(a, b)
#define ccexp(z) cexp(z)
#define ccadd(a, b) cadd(a, b)
#define ccsub(a, b) cdiff(a, b)
#define text_measure(a) ((a) * (2.5))
#else
#define Cplx float _Complex
#define ccabsf(z) cabsf(z)
#define cfromreal(re) (re)
#define cfromimag(im) ((im) * I)
#define cmul(a, b) ((a) * (b))
#define ccexp(z) cexp(z)
#define ccadd(a, b) ((a) + (b))
#define ccsub(a, b) ((a) - (b))
#define text_measure(a) ((a) * (2))
#endif

// Ui Define
#if defined(PLATFORM_WEB) // DEFINE PLATFORM_WEB
#define size_music_rec_t 12
#define TEXT_SIZE 20
#elif defined(PLATFORM_ANDROID)
#define size_music_rec_t 30
#define TEXT_SIZE 30
#else
#define size_music_rec_t 12
#define TEXT_SIZE 18
#endif // END IF

typedef struct {
  char *filepath;
} FilePath;

typedef struct {
  FilePath *file;
  char *current_music_path;
  int file_count;
  Music music;
  float volume, timeplayed, frameTime, smoothness, outLog[BUFFER_SIZE],
      smooth[BUFFER_SIZE], window[BUFFER_SIZE], in[BUFFER_SIZE];
  Cplx out[BUFFER_SIZE];
  bool menu;
} MusicVisualizer;

void create_music();
void update_draw_frame(ScreenVisualizer *sv);
void destroy_music();
void free_file_path();

#endif // !VISUAL_H
