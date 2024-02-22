#include "visual.h"

MusicVisualizer mv = {
    .volume = 0.2f,
    .timeplayed = 0.0f,
};

float in[N];
float window[N];
float complex out[N];

float Amp(float complex z) {
  // float a = crealf(z);
  // float b = cimagf(z);
  // return logf(a*a + b*b);
  return cabsf(z);
}

void Fft(float in[], size_t stride, float complex out[], size_t n) {
  assert(n > 0);
  if (n == 1) {
    out[0] = in[0];
    return;
  }

  Fft(in, stride * 2, out, n / 2);
  Fft(in + stride, stride * 2, out + n / 2, n / 2);
  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    float complex v = cexp(-2 * I * PI * t) * out[k + n / 2];
    float complex e = out[k];
    out[k] = e + v;
    out[k + n / 2] = e - v;
  }
}

void ProcessAudio(void *buffer, unsigned int frames) {
  float(*frame)[2] = buffer;
  for (size_t i = 0; i < frames; ++i) {
    memmove(in, in + 1, (N - 1) * sizeof(in[0]));
    in[N - 1] = frame[i][0];
  }
}

void ClearSamples() {
  memset(in, 0, N);
  memset(out, 0, N);
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
  if (IsKeyDown(KEY_DOWN) && mv.volume > 0.1f) {
    mv.volume -= 0.1f;
  } else if (IsKeyDown(KEY_UP) && mv.volume < 1.0f) {
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
  for (size_t i = 0; i < N; ++i) {
    float t = (float)i / N;
    float multiplier = 0.5 * (1 - cos(2 * PI * t));
    window[i] = in[i] * multiplier;
  }

  Fft(window, 1, out, N);

  float max_amp = 0.0f;
  for (size_t i = 0; i < N; i++) {
    float c = Amp(out[i]);
    if (max_amp < c)
      max_amp = c;
  }

  float step = 1.06;
  float lowFreq = 1.0f;
  size_t m = 0;
  for (float freq = lowFreq; (size_t)freq < N / 2; freq = ceilf(freq * step)) {
    m += 1;
  }

  float cellWidth = (float)sv->screenWidth / m;
  m = 0;

  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (float freq = lowFreq; (size_t)freq < N / 2; freq = ceilf(freq * step)) {
    float f = ceilf(freq * step);
    float a = 0.0f;
    for (size_t q = (size_t)freq; q < N / 2 && q < (size_t)f; ++q) {
      float b = Amp(out[q]);
      if (b > a)
        a = b;
    }
    float t = a / max_amp;
    DrawRectangle(m * cellWidth,
                  (float)(sv->screenHeight - 40) -
                      (float)sv->screenHeight / 2 * t,
                  cellWidth, (float)sv->screenHeight / 2 * t, MAROON);
    m += 1;
  }
  DrawRectangle(20, sv->screenHeight - 42, sv->screenWidth - 40, 12, LIGHTGRAY);
  DrawRectangle(20, sv->screenHeight - 42, (int)mv.timeplayed, 12, MAROON);
  DrawRectangleLines(20, sv->screenHeight - 42, sv->screenWidth - 40, 12, GRAY);
  DrawText(TextFormat("%.02d : %.02d - %.02d : %.02d", minutes, seconds,
                      minutesLength, secondsLength),
           20, sv->screenHeight - 27, 20, BLACK);
  DrawText("Press SPACE to PLAY/STOP Music", 40, 40, 20, BLACK);
  DrawText("Press P to pause Music", 40, 70, 20, BLACK);
  DrawText("Press ARROW UP / ARROW DOWN to change volume", 40, 100, 20, BLACK);
  DrawText("Drag Music for play another Music", 40, 130, 20, BLACK);
  DrawText(TextFormat("VOLUME: %.2f%%", mv.volume * 100), 40, 160, 20, MAROON);
  EndDrawing();
}

void DestroyMusic() {
  DetachAudioStreamProcessor(mv.music.stream, ProcessAudio);
  UnloadMusicStream(mv.music);
  CloseAudioDevice();
}
