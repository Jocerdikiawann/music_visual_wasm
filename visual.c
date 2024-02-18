#include "visual.h"

void CreateMusic(MusicVisualizer *mv) {

  InitAudioDevice();

  mv->music = LoadMusicStream("./videoplayback.ogg");

  mv->music.looping = false;
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
