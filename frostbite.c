#include "raylib.h"

int main(void) {
    InitWindow(1366,768, "Frostbite Effect - GWENT Style");
    SetTargetFPS(60);
    Texture2D menuBG=LoadTexture("main menu.jpg");
    Texture2D frost=LoadTexture("frost.jpg");

    while (!WindowShouldClose()) {
        DrawTexture(menuBG,0,0,WHITE);
        DrawTexture(frost,0,0,Fade(WHITE,0.5f));
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
