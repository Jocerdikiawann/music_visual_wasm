#include <raylib.h>

int main() {
  InitWindow(800, 450, "Music Visualizer");

  InitAudioDevice();

  Sound sound = LoadSound("videoplayback.ogg");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_SPACE))
      PlaySound(sound);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Press SPACE to play sound", 200, 100, 20, LIGHTGRAY);
    EndDrawing();
  }

  UnloadSound(sound);

  CloseAudioDevice();
  CloseWindow();

  return 0;
}
