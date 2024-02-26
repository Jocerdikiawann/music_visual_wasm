#include "visual.h"

typedef float complex Cplx;
typedef struct {
  Music music;
  float volume, timeplayed, frameTime, smoothness, outLog[BUFFER_SIZE],
      smooth[BUFFER_SIZE], window[BUFFER_SIZE], in[BUFFER_SIZE];
  Cplx out[BUFFER_SIZE];
} MusicVisualizer;

MusicVisualizer mv = {
    .volume = 0.2f,
    .timeplayed = 0.0f,
    .smoothness = 8.,
};

float Amp(Cplx z) {
  float a = crealf(z);
  float b = cimagf(z);
  return logf(a * a + b * b);
  // return cabsf(z);
}

void Fft(float in[], size_t stride, Cplx out[], size_t n) {
  assert(n > 0);
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  Fft(in, stride * 2, out, n / 2);
  Fft(in + stride, stride * 2, out + n / 2, n / 2);
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    Cplx v = cexp(-2 * I * PI * t) * out[k + n / 2];
    Cplx e = out[k];
    out[k] = e + v;
    out[k + n / 2] = e - v;
  }
}

void ProcessAudio(void *buffer, unsigned int frames) {
  float(*frame)[2] = buffer;
  for (size_t i = 0; i < frames; ++i) {
    memmove(mv.in, mv.in + 1, (BUFFER_SIZE - 1) * sizeof(mv.in[0]));
    mv.in[BUFFER_SIZE - 1] = frame[i][0];
  }
}

void ClearSamples() {
  memset(mv.in, 0, sizeof(mv.in));
  memset(mv.out, 0, sizeof(mv.out));
  memset(mv.outLog, 0, sizeof(mv.outLog));
  memset(mv.window, 0, sizeof(mv.window));
  memset(mv.smooth, 0, sizeof(mv.smooth));
}

void CreateMusic() {

  InitAudioDevice();

  mv.music = LoadMusicStream("./videoplayback.ogg");

  mv.music.looping = false;

  AttachAudioStreamProcessor(mv.music.stream, ProcessAudio);
}

void DragAndDropFiles() {
  FilePathList droppedFiles = LoadDroppedFiles();
  if (droppedFiles.count <= 0) {
    UnloadDroppedFiles(droppedFiles);
    return;
  }
  ClearSamples();
  if (IsMusicStreamPlaying(mv.music)) {
    StopMusicStream(mv.music);
    DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
    UnloadMusicStream(mv.music);
  }
  char *path = droppedFiles.paths[0];
  if (IsFileExtension(path, ".ogg") || IsFileExtension(path, ".mp3")) {
    mv.music = LoadMusicStream(path);
    mv.music.looping = false;
    AttachAudioStreamProcessor(mv.music.stream, ProcessAudio);
    PlayMusicStream(mv.music);
  }
  UnloadDroppedFiles(droppedFiles);
}

void UpdateDrawFrame(ScreenVisualizer *sv) {
  UpdateMusicStream(mv.music);
  if (IsFileDropped()) {
    DragAndDropFiles();
  }
  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicStreamPlaying(mv.music)) {
      StopMusicStream(mv.music);
      DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
      ClearSamples();
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

  if (IsKeyPressed(KEY_DOWN) && mv.volume > 0.1f) {
    mv.volume -= 0.1f;
  } else if (IsKeyPressed(KEY_UP) && mv.volume < 1.0f) {
    mv.volume += 0.1f;
  }

  SetMusicVolume(mv.music, mv.volume);

  mv.timeplayed = GetMusicTimePlayed(mv.music) / GetMusicTimeLength(mv.music) *
                  (sv->screenWidth - 40);
  int minutes = GetMusicTimePlayed(mv.music) / 60;
  int seconds = GetMusicTimePlayed(mv.music) - (minutes * 60);

  int minutesLength = GetMusicTimeLength(mv.music) / 60;
  int secondsLength = GetMusicTimeLength(mv.music) - (minutesLength * 60);

  // Hann Windowing :
  // https://stackoverflow.com/questions/3555318/implement-hann-window
  for (size_t i = 0; i < BUFFER_SIZE; ++i) {
    float t = (float)i / BUFFER_SIZE;
    float multiplier = 0.5 * (1 - cos(2 * PI * t));
    mv.window[i] = mv.in[i] * multiplier;
  }

  Fft(mv.window, 1, mv.out, BUFFER_SIZE);

  float max_amp = 1.0f;
  float step = 1.06;
  float lowFreq = 1.0f;
  size_t m = 0;
  for (float freq = lowFreq; (size_t)freq < BUFFER_SIZE / 2;
       freq = ceilf(freq * step)) {
    float f = ceilf(freq * step);
    float a = 0.0f;
    for (size_t q = (size_t)freq; q < BUFFER_SIZE / 2 && q < (size_t)f; ++q) {
      float b = Amp(mv.out[q]);
      if (b > a)
        a = b;
    }
    if (max_amp < a)
      max_amp = a;
    mv.outLog[m++] = a;
  }

  float frameTime = GetFrameTime();
  for (size_t i = 0; i < m; ++i) {
    mv.outLog[i] /= max_amp;

    mv.smooth[i] += (mv.outLog[i] - mv.smooth[i]) * mv.smoothness * frameTime;
  }

  float cellWidth = (float)sv->screenWidth / m;
  BeginDrawing();
  ClearBackground(RAYWHITE);

  DrawText("Press SPACE to PLAY/STOP Music", 20, 60, TEXT_SIZE, BLACK);
  DrawText("Press P to pause Music", 20, 80, TEXT_SIZE, BLACK);
  DrawText("Press ARROW UP / ARROW DOWN to change volume", 20, 100, TEXT_SIZE,
           BLACK);
  DrawText("Drag Music for play another Music", 20, 120, TEXT_SIZE, BLACK);

  DrawText(TextFormat("VOLUME: %.2f%%", mv.volume * 100), 20, 140, TEXT_SIZE,
           MAROON);
  DrawText(TextFormat("%.02d : %.02d - %.02d : %.02d", minutes, seconds,
                      minutesLength, secondsLength),
           20, sv->screenHeight - 25, TEXT_SIZE, BLACK);

  for (size_t i = 1; i <= 3; ++i) {
    DrawRectangle(20, 8 * i, 50, 5, MAROON);
  }
  for (size_t i = 0; i < m; ++i) {
    float t = mv.smooth[i];
    DrawRectangle(i * cellWidth + 20,
                  (sv->screenHeight - 40) - (float)sv->screenHeight / 3 * t,
                  cellWidth - 1, (float)sv->screenHeight / 3 * t, MAROON);
  }
  DrawRectangle(20, sv->screenHeight - 40, sv->screenWidth - 40, 12, LIGHTGRAY);
  DrawRectangle(20, sv->screenHeight - 40, (int)mv.timeplayed, 12, MAROON);
  DrawRectangleLines(20, sv->screenHeight - 40, sv->screenWidth - 40, 12, GRAY);

  EndDrawing();
}

void DestroyMusic() {
  DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
  UnloadMusicStream(mv.music);
  CloseAudioDevice();
}
