#include "visual.h"

double pi;
typedef double complex cplx;

void _fft(cplx buf[], cplx out[], int n, int step) {
  if (step < n) {
    _fft(out, buf, n, step * 2);
    _fft(out + step, buf + step, n, step * 2);
    for (int i = 0; i < n; i += 2 * step) {
      cplx t = cexp(-I * pi * i / n) * out[i + step];
      buf[i / 2] = out[i] + t;
      buf[(i + n) / 2] = out[i] - t;
    }
  }
}

void fft(cplx buf[], int n) {
  cplx out[n];
  for (int i = 0; i < n; i++)
    out[i] = buf[i];
  _fft(buf, out, n, 1);
}

void ConstructWindows(MusicVisualizer *mv) {
  float *temp = (float *)malloc(sizeof(float) * mv->sampleBufferSize);
  if (temp == NULL) {
    printf("Failed allocate memory\n");
    exit(1);
  }
  for (int i = 0; i < mv->sampleBufferSize; i++) {
    temp[i] = 0.54 - 0.46 * cos(2 * M_PI * i / (float)mv->sampleBufferSize);
  }
  mv->windowCache = temp;
  free(temp);
  temp = NULL;
}

void CreateMusic(MusicVisualizer *mv) {

  InitAudioDevice();
  mv->sampleBufferSize = BUFFER_SIZE;

  mv->music = LoadMusicStream("./videoplayback.ogg");

  mv->music.looping = false;
  mv->sampleRate = mv->music.stream.sampleRate * mv->music.stream.channels;
  if (mv->sampleBufferSize > mv->music.stream.sampleSize) {
    mv->sampleBufferSize = mv->music.stream.sampleSize;
  }

  ConstructWindows(mv);
}

void UpdateDrawFrame(MusicVisualizer *mv, ScreenVisualizer *sv) {
  UpdateMusicStream(mv->music);

  if (IsKeyPressed(KEY_SPACE)) {
    StopMusicStream(mv->music);
    PlayMusicStream(mv->music);
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
