#include "visual.h"

#ifdef PLATFORM_ANDROID
#define size_music_rec_t 30
#define Cplx complex_t
#define ccabsf(z) ccabs(z)
#define cfromreal(re) cvalue(re, 0)
#define cfromimag(im) cvalue(0, im)
#define cmul(a, b) cmultiply(a, b)
#define ccexp(z) cexp(z)
#define ccadd(a, b) cadd(a, b)
#define ccsub(a, b) cdiff(a, b)
#else
#define size_music_rec_t 12
#define Cplx float _Complex
#define ccabsf(z) cabsf(z)
#define cfromreal(re) (re)
#define cfromimag(im) ((im) * I)
#define cmul(a, b) ((a) * (b))
#define ccexp(z) cexp(z)
#define ccadd(a, b) ((a) + (b))
#define ccsub(a, b) ((a) - (b))
#endif

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

MusicVisualizer mv = {
    .volume = 0.5f,
    .timeplayed = 0.0f,
    .smoothness = 6.,
};

float amp(Cplx z) { return logf(ccabsf(z)); }

void fft(float in[], size_t stride, Cplx out[], size_t n) {
  assert(n > 0);
  if (n == 1) {
    out[0] = cfromreal(in[0]);
    return;
  }

  fft(in, stride * 2, out, n / 2);
  fft(in + stride, stride * 2, out + n / 2, n / 2);

  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    Cplx v = cmul(ccexp(cfromimag(-2 * PI * t)), out[k + n / 2]);
    Cplx e = out[k];
    out[k] = ccadd(e, v);
    out[k + n / 2] = ccsub(e, v);
  }
}

void process_audio(void *buffer, unsigned int frames) {
  float(*frame)[2] = buffer;
  for (size_t i = 0; i < frames; ++i) {
    memmove(mv.in, mv.in + 1, (BUFFER_SIZE - 1) * sizeof(mv.in[0]));
    mv.in[BUFFER_SIZE - 1] = frame[i][0];
  }
}

static int fft_analyze() {
  // Hann Windowing :
  // https://stackoverflow.com/questions/3555318/implement-hann-window
  for (size_t i = 0; i < BUFFER_SIZE; ++i) {
    float t = (float)i / BUFFER_SIZE - 1;
    float multiplier = 0.5 * (1 - cos(2 * PI * t));
    mv.window[i] = mv.in[i] * multiplier;
  }

  fft(mv.window, 1, mv.out, BUFFER_SIZE);

  float max_amp = 1.0f;
  float step = 1.06;
  float lowFreq = 1.0f;
  size_t m = 0;

  for (float freq = lowFreq; (size_t)freq < BUFFER_SIZE / 2;
       freq = ceilf(freq * step)) {
    float f = ceilf(freq * step);
    float a = 0.0f;
    for (size_t q = (size_t)freq; q < BUFFER_SIZE / 2 && q < (size_t)f; ++q) {
      float b = amp(mv.out[q]);
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
  return m;
}

void create_music() {

  InitAudioDevice();

  mv.file = (FilePath *)malloc(sizeof(FilePath) * MAX_FILEPATH_RECORDED);
  if (mv.file == NULL) {
    printf("Failed Allocate memory at %d\n", __LINE__);
    exit(1);
  }

  char *path = "idol.ogg";

  mv.file[0].filepath = path;
  mv.file_count = 1;
  mv.current_music_path = path;

  mv.music = LoadMusicStream(mv.file[0].filepath);

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

void load_music(char *filepath) {
  mv.music = LoadMusicStream(filepath);
  mv.current_music_path = filepath;
}

void play_music() {
  mv.music.looping = false;
  AttachAudioStreamProcessor(mv.music.stream, process_audio);
  PlayMusicStream(mv.music);
}

void open_menu(ScreenVisualizer *sv) {
  if (mv.menu) {
    DrawRectangle(20, 35, sv->screenWidth / 3, sv->screenHeight * 0.8,
                  RAYWHITE);
    DrawRectangleLines(20, 35, (sv->screenWidth / 3), sv->screenHeight * 0.8,
                       BLACK);
    for (int i = 0, offset = 1; i < mv.file_count; ++i, ++offset) {
      Rectangle file_item_boundary = {
          .x = 25,
          .y = 40 * offset,
          .width = sv->screenWidth * 0.3,
          .height = 25,
      };

      Color box_color = LIGHTGRAY, text_color = BLACK;
      if (CheckCollisionPointRec(GetMousePosition(), file_item_boundary)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          clear_samples();
          if (IsMusicStreamPlaying(mv.music))
            stop_music();
          if (mv.current_music_path != mv.file[i].filepath)
            load_music(mv.file[i].filepath);
          play_music();
        }
        box_color = MAROON, text_color = RAYWHITE;
      }
      DrawRectangleRec(file_item_boundary, box_color);

      const char *filename = GetFileName(mv.file[i].filepath);
      char copystring[23];
      if (TextLength(filename) > 23)
        strncpy(copystring, filename, 23);

      DrawText(TextFormat("%s", TextLength(filename) >
                                        (unsigned int)file_item_boundary.x
                                    ? copystring
                                    : filename),
               file_item_boundary.x + 5, file_item_boundary.y + 5, TEXT_SIZE,
               text_color);
    }
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
    DrawRectangleRec(burger_item_boundary, mv.menu ? MAROON : burger_color);
  }
}

void drag_and_drop_files() {
  FilePathList droppedFiles = LoadDroppedFiles();
  if (droppedFiles.count <= 0) {
    TraceLog(LOG_ERROR, "Empty file dropped");
    UnloadDroppedFiles(droppedFiles);
    return;
  }
  if (droppedFiles.count >= 100 && mv.file_count == 100) {
    FilePath *new_file_path =
        (FilePath *)realloc(mv.file, sizeof(FilePath) * MAX_FILEPATH_RECORDED);
    if (new_file_path == NULL) {
      TraceLog(LOG_ERROR, "Failed reallocate memory");
    } else {
      mv.file = new_file_path;
    }
  }
  for (size_t i = 0; i < droppedFiles.count; ++i) {
    char *path = strdup(droppedFiles.paths[i]);
    if (IsFileExtension(path, ".ogg") || IsFileExtension(path, ".mp3")) {
      mv.file[mv.file_count].filepath = path;
      mv.file_count++;
    } else {
      // TODO: show error when file not support
      free(path);
      TraceLog(LOG_ERROR, "Couldn't load file");
    }
  }

  UnloadDroppedFiles(droppedFiles);
}

void action_key() {
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
}

void draw_text_option(ScreenVisualizer *sv) {
  int minutes = GetMusicTimePlayed(mv.music) / 60;
  int seconds = GetMusicTimePlayed(mv.music) - (minutes * 60);

  int minutesLength = GetMusicTimeLength(mv.music) / 60;
  int secondsLength = GetMusicTimeLength(mv.music) - (minutesLength * 60);

  // TODO:MEASURE TEXT
  DrawText("Press SPACE to PLAY/STOP Music", 20, sv->screenHeight * 0.5,
           TEXT_SIZE, BLACK);
  // DrawText("Press P to pause Music", 20, 60 * 2, TEXT_SIZE, BLACK);
  // DrawText("Press ARROW UP / ARROW DOWN to change volume", 20, 60 * 3,
  //          TEXT_SIZE, BLACK);
  // DrawText("Drag Music for play another Music", 20, 60 * 4, TEXT_SIZE,
  // BLACK); DrawText(TextFormat("VOLUME: %.2f%%", mv.volume * 100), 20, 60 * 5,
  // TEXT_SIZE,
  //          MAROON);
  // DrawText(TextFormat("%.02d : %.02d - %.02d : %.02d", minutes, seconds,
  //                     minutesLength, secondsLength),
  //          20, sv->screenHeight - 25, TEXT_SIZE, BLACK);
}

void update_draw_frame(ScreenVisualizer *sv) {
  UpdateMusicStream(mv.music);
  sv->screenWidth = GetScreenWidth(), sv->screenHeight = GetScreenHeight();
  if (IsFileDropped()) {
    drag_and_drop_files();
  }

  action_key();

  SetMusicVolume(mv.music, mv.volume);

  mv.timeplayed = GetMusicTimePlayed(mv.music) / GetMusicTimeLength(mv.music) *
                  (sv->screenWidth - 40);

  size_t m = fft_analyze();

  float cellWidth = (float)sv->screenWidth / m;
  BeginDrawing();
  ClearBackground(RAYWHITE);

  if (mv.file_count == 0) {
    DrawText("Drop Music", sv->screenHeight / 2, sv->screenWidth / 2, TEXT_SIZE,
             BLACK);
  } else {
    draw_text_option(sv);

    for (size_t i = 0; i < m; ++i) {
      float t = mv.smooth[i];
      DrawRectangle(i * cellWidth + 20,
                    (sv->screenHeight - 40) - (float)sv->screenHeight / 3 * t,
                    cellWidth - 1, (float)sv->screenHeight / 3 * t, MAROON);
    }

    DrawRectangle(20, sv->screenHeight - 40, sv->screenWidth - 40,
                  size_music_rec_t, LIGHTGRAY);
    DrawRectangle(20, sv->screenHeight - 40, (int)mv.timeplayed,
                  size_music_rec_t, MAROON);
    DrawRectangleLines(20, sv->screenHeight - 40, sv->screenWidth - 40,
                       size_music_rec_t, GRAY);
    draw_menu();
    open_menu(sv);
  }

  EndDrawing();
}

void destroy_music() {
  DetachAudioStreamProcessor(mv.music.stream, process_audio);
  UnloadMusicStream(mv.music);
  CloseAudioDevice();
}

void free_file_path() {
  free(mv.file);
  mv.file = NULL;
}
