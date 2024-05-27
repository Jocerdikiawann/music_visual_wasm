#include "visual.h"

MusicVisualizer mv = {
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
  AttachAudioStreamProcessor(mv.music.stream, process_audio);
  mv.music.looping = false;
  PlayMusicStream(mv.music);
}

void create_music() {

  InitAudioDevice();
  mv.volume = GetMasterVolume();

  mv.file = (FilePath *)malloc(sizeof(FilePath) * MAX_FILEPATH_RECORDED);
  if (mv.file == NULL) {
    printf("Failed Allocate memory at %d\n", __LINE__);
    exit(1);
  }

  clear_samples();

  char *path[] = {"zenzen.ogg", "idol.ogg", "videoplayback.ogg", "porty.ogg"};
  for (size_t i = 0; i < 4; ++i) {
    mv.file[i].filepath = path[i];
    mv.file_count = i;
  }

  mv.current_music_path = path[0];
  load_music(mv.file[mv.file_count - 1].filepath);
}

void open_menu(ScreenVisualizer *sv) {
  int pos_y_menu = 35;
#ifdef PLATFORM_ANDROID
  pos_y_menu = 20 + ((8 * 3) * 2);
#endif
  Rectangle menu_box_boundary = {
      .width = (float)sv->screenWidth / 3,
      .height = sv->screenHeight * 0.8,
      .x = 20,
      .y = pos_y_menu,
  };

  if (mv.menu) {
    DrawRectangleRec(menu_box_boundary, RAYWHITE);
    DrawRectangleLinesEx(menu_box_boundary, 1, BLACK);

    for (int i = 0, offset = 1; i < mv.file_count; ++i, ++offset) {
      Rectangle file_item_boundary = {
          .x = 25,
          .y = (menu_box_boundary.y + 5) * offset,
          .width = sv->screenWidth * 0.3,
          .height = TEXT_SIZE + 10,
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

void draw_menu(ScreenVisualizer *sv) {
  int w = sv->screenWidth * 0.05, h = sv->screenHeight * .009;

  for (size_t i = 1; i <= 3; ++i) {
    Rectangle burger_item_boundary = {
        .x = 20,
        .y = 8,
        .width = w,
        .height = h,
    };
    Color burger_color = LIGHTGRAY;
#ifdef PLATFORM_ANDROID
    burger_item_boundary.y *= (i * 2);
#elif PLATFORM_WEB

    burger_item_boundary.y *= (i * 2);
#else
    if (IsWindowMaximized()) {
      burger_item_boundary.y *= (i * 2);
    } else {
      burger_item_boundary.y *= i;
    }
#endif
    if (GetMousePosition().y <=
            burger_item_boundary.height * burger_item_boundary.height &&
        GetMousePosition().y >= burger_item_boundary.height &&
        GetMousePosition().x >= burger_item_boundary.x &&
        GetMousePosition().x <= burger_item_boundary.width + 1) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && i <= 3) {
        // open_dir();
        mv.menu = !mv.menu;
      }
      burger_color = MAROON;
    }

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

  int pos_y_menu = 60;
  int pos_y_volume = sv->screenHeight - 25;
#ifdef PLATFORM_ANDROID
  pos_y_menu = 20 + ((8 * 3) * 2);
  pos_y_volume = sv->screenHeight - 50;
#endif
  DrawText("Press SPACE to PLAY/STOP Music\nPress P to pause Music\nPress "
           "ARROW UP / ARROW DOWN to change volume\n",
           20, pos_y_menu, TEXT_SIZE, BLACK);
  DrawText(TextFormat("VOLUME: %.2f%%", mv.volume * 100), 20,
           text_measure(pos_y_menu), TEXT_SIZE, MAROON);
  DrawText(TextFormat("%.02d : %.02d - %.02d : %.02d", minutes, seconds,
                      minutesLength, secondsLength),
           20, pos_y_volume, TEXT_SIZE, BLACK);
}

void update_draw_frame(ScreenVisualizer *sv) {
  sv->screenWidth = GetScreenWidth(), sv->screenHeight = GetScreenHeight();

  UpdateMusicStream(mv.music);

  if (IsFileDropped()) {
    drag_and_drop_files();
  }

  action_key();

  SetMasterVolume(mv.volume);

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

    int post_y_track = sv->screenHeight - 40;
#ifdef PLATFORM_ANDROID
    post_y_track = sv->screenHeight - 80;
#endif
    for (size_t i = 0; i < m; ++i) {
      float t = mv.smooth[i];
      DrawRectangle(i * cellWidth + 20,
                    (post_y_track) - (float)sv->screenHeight / 3 * t,
                    cellWidth - 1, (float)sv->screenHeight / 3 * t, MAROON);
    }

    DrawRectangle(20, post_y_track, sv->screenWidth - 40, size_music_rec_t,
                  LIGHTGRAY);
    DrawRectangle(20, post_y_track, (int)mv.timeplayed, size_music_rec_t,
                  MAROON);
    DrawRectangleLines(20, post_y_track, sv->screenWidth - 40, size_music_rec_t,
                       GRAY);

    draw_menu(sv);
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
