#include <raylib.h>
#include <stdbool.h>

int main() {
  int screenWidth = 800;
  int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Music Visualizer");

  InitAudioDevice();

  Music music = LoadMusicStream("./videoplayback.ogg");

  music.looping = false;

  SetTargetFPS(60);

  float volume = 0.5f;
  float timeplayed = 0.0f;
  bool pause = false;

  while (!WindowShouldClose()) {
    UpdateMusicStream(music);

    if (IsKeyPressed(KEY_SPACE)) {
      StopMusicStream(music);
      PlayMusicStream(music);
    }

    if (IsKeyPressed(KEY_P)) {
      pause = !pause;
      if (pause)
        PauseMusicStream(music);
      else
        ResumeMusicStream(music);
    }

    if (IsKeyDown(KEY_DOWN)) {
      volume -= 0.1f;
    } else if (IsKeyDown(KEY_UP)) {
      volume += 0.1f;
    }

    SetMusicVolume(music, volume);

    timeplayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music) *
                 (screenWidth - 40);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(20, screenHeight - 20 - 12, screenWidth - 40, 12, LIGHTGRAY);
    DrawRectangle(20, screenHeight - 20 - 12, (int)timeplayed, 12, MAROON);
    DrawRectangleLines(20, screenHeight - 20 - 12, screenWidth - 40, 12, GRAY);
    DrawText("Press SPACE to PLAY/STOP sound", 40, 40, 20, BLACK);
    DrawText("Press P to pause sound", 40, 70, 20, BLACK);
    DrawText("Press ARROW UP / ARROW DOWN to change volume", 40, 100, 20,
             BLACK);
    DrawText(TextFormat("VOLUME: %f", volume), 40, 130, 20, MAROON);
    EndDrawing();
  }

  UnloadMusicStream(music);

  CloseAudioDevice();
  CloseWindow();

  return 0;
}
