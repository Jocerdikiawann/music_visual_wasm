#ifndef EXT_H
#define EXT_H
#if defined(PLATFORM_ANDROID)
#define MINIAUDIO_IMPLEMENTATION
#endif
#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif /* ifdef  defined(PLATFORM_WEB) */
#include "assert.h"
#include <complex.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif // !EXT_H
