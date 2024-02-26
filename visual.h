#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define BUFFER_SIZE (1 << 13)
#ifdef PLATFORM_WEB
#define TEXT_SIZE 20
#else
#define TEXT_SIZE 18
#endif

void CreateMusic();
void UpdateDrawFrame(ScreenVisualizer *sv);
void DestroyMusic();

#endif // !VISUAL_H
