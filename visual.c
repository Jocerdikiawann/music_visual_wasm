#include "visual.h"

typedef float complex Cplx;

typedef struct {
  Music music;
  float volume, timeplayed, frameTime, smoothness, outLog[BUFFER_SIZE],
      smooth[BUFFER_SIZE], window[BUFFER_SIZE], in[BUFFER_SIZE];
  Cplx out[BUFFER_SIZE];
  bool menu;
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

void process_audio(void *buffer, unsigned int frames) {
  float(*frame)[2] = buffer;
  for (size_t i = 0; i < frames; ++i) {
    memmove(mv.in, mv.in + 1, (BUFFER_SIZE - 1) * sizeof(mv.in[0]));
    mv.in[BUFFER_SIZE - 1] = frame[i][0];
  }
}

void CreateMusic() {

  InitAudioDevice();

  mv.music = LoadMusicStream("./videoplayback.ogg");

  mv.music.looping = false;

  AttachAudioStreamProcessor(mv.music.stream, process_audio);
}

void clear_samples() {
  memset(mv.in, 0, sizeof(mv.in));
  memset(mv.out, 0, sizeof(mv.out));
  memset(mv.outLog, 0, sizeof(mv.outLog));
  memset(mv.window, 0, sizeof(mv.window));
  memset(mv.smooth, 0, sizeof(mv.smooth));
}

void stop_music() {
  StopMusicStream(mv.music);
  DetachAudioStreamProcessor(mv.music.stream, process_audio);
  clear_samples();
}

void play_music() {
  AttachAudioStreamProcessor(mv.music.stream, process_audio);
  PlayMusicStream(mv.music);
}

void open_menu(ScreenVisualizer *sv) {
  if (mv.menu) {
    DrawRectangle(20, 35, sv->screenWidth / 3, sv->screenHeight * 0.8,
                  LIGHTGRAY);
    DrawRectangleLines(20, 35, (sv->screenWidth / 3), sv->screenHeight * 0.8,
                       BLACK);
  }
}

void draw_menu() {
  for (size_t i = 1; i <= 3; ++i) {
    Rectangle burger_item_boundary = {
        .x = 20,
        .y = 8,
        .width = 50,
        .height = 5,
    };
    Color burger_color = LIGHTGRAY;
    if (GetMousePosition().y <= burger_item_boundary.y * 3 &&
        GetMousePosition().y >= burger_item_boundary.y &&
        GetMousePosition().x >= burger_item_boundary.x &&
        GetMousePosition().x <= burger_item_boundary.width + 1) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && i == 1) {
        mv.menu = !mv.menu;
      }
      burger_color = MAROON;
    }
    burger_item_boundary.y *= i;
    DrawRectangleRec(burger_item_boundary, burger_color);
  }
}

void drag_and_drop_files() {
  FilePathList droppedFiles = LoadDroppedFiles();
  if (droppedFiles.count <= 0) {
    UnloadDroppedFiles(droppedFiles);
    return;
  }
  clear_samples();
  if (IsMusicStreamPlaying(mv.music)) {
    stop_music();
    UnloadMusicStream(mv.music);
  }
  char *path = droppedFiles.paths[0];
  if (IsFileExtension(path, ".ogg") || IsFileExtension(path, ".mp3")) {
    mv.music = LoadMusicStream(path);
    mv.music.looping = false;
    play_music();
  }
  UnloadDroppedFiles(droppedFiles);
}

void UpdateDrawFrame(ScreenVisualizer *sv) {
  UpdateMusicStream(mv.music);
  if (IsFileDropped()) {
    drag_and_drop_files();
  }
  if (IsKeyPressed(KEY_SPACE)) {
    if (IsMusicStreamPlaying(mv.music)) {
      stop_music();
    } else {
      play_music();
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

  for (size_t i = 0; i < m; ++i) {
    float t = mv.smooth[i];
    DrawRectangle(i * cellWidth + 20,
                  (sv->screenHeight - 40) - (float)sv->screenHeight / 3 * t,
                  cellWidth - 1, (float)sv->screenHeight / 3 * t, MAROON);
  }
  DrawRectangle(20, sv->screenHeight - 40, sv->screenWidth - 40, 12, LIGHTGRAY);
  DrawRectangle(20, sv->screenHeight - 40, (int)mv.timeplayed, 12, MAROON);
  DrawRectangleLines(20, sv->screenHeight - 40, sv->screenWidth - 40, 12, GRAY);
  draw_menu();
  open_menu(sv);
  EndDrawing();
}

void DestroyMusic() {
  DetachAudioStreamProcessor(mv.music.stream, process_audio);
  UnloadMusicStream(mv.music);
  CloseAudioDevice();
}
