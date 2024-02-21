#include "visual.h"

MusicVisualizer mv = {
    .volume = 0.2f,
    .timeplayed = 0.0f,
};

float in[N];
float complex out[N];

float amp(float complex z) {
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  if (a < b)
    return b;
  return a;
}

void fft(float in[], size_t stride, float complex out[], size_t n) {
  assert(n > 0);
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  fft(in, stride * 2, out, n / 2);
  fft(in + stride, stride * 2, out + n / 2, n / 2);
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    float complex v = cexp(-2 * I * PI * t) * out[k + n / 2];
    float complex e = out[k];
    out[k] = e + v;
    out[k + n / 2] = e - v;
  }
}

void ProcessAudio(void *buffer, unsigned int frames) {
  float(*frame)[mv.music.stream.channels] = buffer;
  for (size_t i = 0; i < frames; ++i) {
    memmove(in, in + 1, (N - 1) * sizeof(in[0]));
    in[N - 1] = frame[i][0];
  }
}

void CreateMusic() {

  InitAudioDevice();

  mv.music = LoadMusicStream("./videoplayback.ogg");

  mv.music.looping = false;

  AttachAudioStreamProcessor(mv.music.stream, ProcessAudio);
}

void DragAndDropFiles(MusicVisualizer *mv) {}

void UpdateDrawFrame(ScreenVisualizer *sv) {
  UpdateMusicStream(mv.music);

  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicStreamPlaying(mv.music)) {
      StopMusicStream(mv.music);
      DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
    } else {
      AttachAudioStreamProcessor(mv.music.stream, ProcessAudio);
      PlayMusicStream(mv.music);
    }
  }

  if (IsKeyPressed(KEY_P)) {
    if (IsMusicStreamPlaying(mv.music))
      PauseMusicStream(mv.music);
    else
      ResumeMusicStream(mv.music);
  }
  if (IsKeyDown(KEY_DOWN) && mv.volume > 0.1f) {
    mv.volume -= 0.1f;
  } else if (IsKeyDown(KEY_UP) && mv.volume < 1.0f) {
    mv.volume += 0.1f;
  }

  SetMusicVolume(mv.music, mv.volume);

  mv.timeplayed = GetMusicTimePlayed(mv.music) / GetMusicTimeLength(mv.music) *
                  (sv->screenWidth - 40);
  fft(in, 1, out, N);

  float max_amp = 0.0f;
  for (size_t i = 0; i < N; i++) {
    float c = amp(out[i]);
    if (max_amp < c)
      max_amp = c;
  }

  float step = 1.06;
  size_t m = 0;
  for (float freq = 20.0f; (size_t)freq < N; freq *= step) {
    m += 1;
  }

  float cellWidth = (float)sv->screenWidth / m;
  m = 0;

  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (float freq = 20.0f; (size_t)freq < N; freq *= step) {
    float f = freq * step;
    float a = 0.0f;
    for (size_t q = (size_t)freq; q < N && q < (size_t)f; ++q) {
      a += amp(out[q]);
    }
    a /= (size_t)f - (size_t)freq + 1;
    float t = a / max_amp;
    DrawRectangle(m * cellWidth,
                  (float)(sv->screenHeight - 30) -
                      (float)sv->screenHeight / 2 * t,
                  cellWidth, (float)sv->screenHeight / 2 * t, MAROON);
    m += 1;
  }
  DrawRectangle(20, sv->screenHeight - 20 - 12, sv->screenWidth - 40, 12,
                LIGHTGRAY);
  DrawRectangle(20, sv->screenHeight - 20 - 12, (int)mv.timeplayed, 12, MAROON);
  DrawRectangleLines(20, sv->screenHeight - 20 - 12, sv->screenWidth - 40, 12,
                     GRAY);
  DrawText("Press SPACE to PLAY/STOP sound", 40, 40, 20, BLACK);
  DrawText("Press P to pause sound", 40, 70, 20, BLACK);
  DrawText("Press ARROW UP / ARROW DOWN to change volume", 40, 100, 20, BLACK);
  DrawText(TextFormat("VOLUME: %.2f%%", mv.volume * 100), 40, 130, 20, MAROON);
  EndDrawing();
}

void DestroyMusic() {
  DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
  UnloadMusicStream(mv.music);
  CloseAudioDevice();
}
