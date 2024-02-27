#ifndef VISUAL_H
#define VISUAL_H

#include "ext.h"

#include "screen.h"

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])
#define BUFFER_SIZE (1 << 13)
#define MAX_FILEPATH_RECORDED 100

#if defined(PLATFORM_WEB) // DEFINE PLATFORM_WEB
#define TEXT_SIZE 20
#else
#define TEXT_SIZE 18
#endif // END IF

void create_music();
void update_draw_frame(ScreenVisualizer *sv);
void destroy_music();
void free_file_path();

#endif // !VISUAL_H
