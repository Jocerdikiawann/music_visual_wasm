#include "visual.h"

float pi;
float in[N];
float complex out[N];
float max_amp;

typedef struct {
  float left, right;
} Frame;

float amp(float complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  if (a < b)
    return b;
  return a;
}

void _fft(float complex buf[], float complex out[], int n, int step) {
  if (step < n) {
    _fft(out, buf, n / 2, step * 2);
    _fft(out + step, buf + step / 2, n / 2, step * 2);

    for (int i = 0; i < n; i += 2 * step) {
      double complex t = cexp(-I * PI * i / n) * out[i + step / 2];
      buf[i / 2] = out[i] + t;
      buf[(i + n) / 2] = out[i] - t;
    }
  }
}

void fft(float complex buf[], int n) {
  float complex out[n];
  for (int i = 0; i < n; i++)
    out[i] = buf[i];

  _fft(buf, out, n, 1);
}

void ProcessAudio(void *buffer, unsigned int frames) {
  if (frames < N)
    return;

  Frame *frame = buffer;
  for (size_t i = 0; i < frames; ++i) {
    in[i] = frame[i].left;
  }
  fft(out, N);

  max_amp = 0.0f;
  for (size_t i = 0; i < N; i++) {

    float c = amp(out[i]);
    if (max_amp < c)
      max_amp = c;
  }
}

void CreateMusic(MusicVisualizer *mv) {

  pi = atan2(1, 1) * 4;
  InitAudioDevice();

  mv->music = LoadMusicStream("./music.ogg");

  mv->music.looping = false;

  AttachAudioStreamProcessor(mv->music.stream, ProcessAudio);
}

void UpdateDrawFrame(MusicVisualizer *mv, ScreenVisualizer *sv) {
  UpdateMusicStream(mv->music);

  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicStreamPlaying(mv->music))
      StopMusicStream(mv->music);
    else {
      PlayMusicStream(mv->music);
    }
  }

  if (IsKeyPressed(KEY_P)) {
    if (IsMusicStreamPlaying(mv->music))
      PauseMusicStream(mv->music);
    else
      ResumeMusicStream(mv->music);
  }

  if (IsKeyDown(KEY_DOWN)) {
    mv->volume -= 0.1f;
  } else if (IsKeyDown(KEY_UP)) {
    mv->volume += 0.1f;
  }

  SetMusicVolume(mv->music, mv->volume);

  mv->timeplayed = GetMusicTimePlayed(mv->music) /
                   GetMusicTimeLength(mv->music) * (sv->screenWidth - 40);

  BeginDrawing();
  ClearBackground(RAYWHITE);
  float cellWidth = (float)sv->screenWidth / N;
  for (size_t i = 0; i < N; ++i) {
    float t = amp(out[i]) / max_amp;
    DrawRectangle(i * cellWidth,
                  (float)sv->screenHeight - 30 -
                      (float)sv->screenHeight / 2 * t,
                  cellWidth, (float)sv->screenHeight / 2 * t, RED);
  }
  DrawRectangle(20, sv->screenHeight - 20 - 12, sv->screenWidth - 40, 12,
                LIGHTGRAY);
  DrawRectangle(20, sv->screenHeight - 20 - 12, (int)mv->timeplayed, 12,
                MAROON);
  DrawRectangleLines(20, sv->screenHeight - 20 - 12, sv->screenWidth - 40, 12,
                     GRAY);
  DrawText("Press SPACE to PLAY/STOP sound", 40, 40, 20, BLACK);
  DrawText("Press P to pause sound", 40, 70, 20, BLACK);
  DrawText("Press ARROW UP / ARROW DOWN to change volume", 40, 100, 20, BLACK);
  DrawText(TextFormat("VOLUME: %f", mv->volume), 40, 130, 20, MAROON);
  EndDrawing();
}

void DestroyMusic(MusicVisualizer *mv) {
  UnloadMusicStream(mv->music);
  CloseAudioDevice();
}
