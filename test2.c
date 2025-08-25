#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "Rotating Image in Raylib");

    Texture2D texture = LoadTexture("mughalSoldier.png");

    float rotation = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        rotation += 1.0f;  // Increase rotation angle

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Define the source rectangle (full texture)
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };

        // Destination rectangle (where and how big to draw it)
        Rectangle destRec = { 400, 300, (float)texture.width, (float)texture.height };

        // Origin = rotation pivot (center of the image)
        Vector2 origin = { (float)texture.width/2, (float)texture.height/2 };

        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
