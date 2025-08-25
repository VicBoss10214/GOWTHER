#include "raylib.h"
#include <stdlib.h>

#define FROST_PARTICLES 200

typedef struct {
    Vector2 pos;
    float radius;
    float speed;
} FrostParticle;

int main(void) {
    InitWindow(800, 600, "Frost Effect - Overlay");
    SetTargetFPS(60);

    FrostParticle frost[FROST_PARTICLES];
    for (int i = 0; i < FROST_PARTICLES; i++) {
        frost[i].pos = (Vector2){ GetRandomValue(0, 800), GetRandomValue(0, 600) };
        frost[i].radius = GetRandomValue(2, 6);
        frost[i].speed = (float)GetRandomValue(10, 40) / 10.0f;
    }

    while (!WindowShouldClose()) {
        // Update
        for (int i = 0; i < FROST_PARTICLES; i++) {
            frost[i].pos.y += frost[i].speed;
            if (frost[i].pos.y > 600) {
                frost[i].pos.y = 0;
                frost[i].pos.x = GetRandomValue(0, 800);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawText("Frost Effect Example", 220, 50, 30, RAYWHITE);

        // Frost overlay
        for (int i = 0; i < FROST_PARTICLES; i++) {
            DrawCircleV(frost[i].pos, frost[i].radius, (Color){200, 200, 255, 100});
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
