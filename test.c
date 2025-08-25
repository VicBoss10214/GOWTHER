#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define MAX_CARDS 48
#define CARD_WIDTH 79
#define CARD_HEIGHT 135
#define GAP 0
#define BASE_SPEED 400
#define TOTAL_GAME_TIME 120.0f
#define MAX_CARDS_PER_LINE 8
#define MAX_LINES 6
#define MAX_BLOCKS 10
#define TURN_TIME 6.0f

typedef struct {
    Color color;
} Card;

// Compare two colors
int CompareColor(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

// Get index of color in mainColors
int getColorIndex(Color c, Color colors[], int count) {
    for (int i = 0; i < count; i++)
        if (CompareColor(c, colors[i]))
            return i;
    return -1;
}

// Apply points according to rules
void applyPoints(Card picked, int playerTurn, int *player1Points, int *player2Points, int blackShieldHolder)
{
    int shieldP1 = (blackShieldHolder == 1);
    int shieldP2 = (blackShieldHolder == 2);

    if (CompareColor(picked.color, RED)) {
        if (playerTurn == 1) *player2Points -= 5;
        else *player1Points -= 5;
    }
    else if (CompareColor(picked.color, BLUE)) {
        if (playerTurn == 1) *player1Points += 15;
        else *player2Points += 15;
    }
    else if (CompareColor(picked.color, ORANGE)) {
        if (playerTurn == 1) *player1Points += 5;
        else *player2Points += 5;
    }
    else if (CompareColor(picked.color, YELLOW)) {
        if (playerTurn == 1) *player1Points += 10;
        else *player2Points += 10;
    }
    else if (CompareColor(picked.color, GREEN)) {
        if (playerTurn == 1) {
            if (!shieldP1) *player1Points -= 3;
            if (!shieldP2) *player2Points -= 3;
        } else {
            if (!shieldP2) *player2Points -= 3;
            if (!shieldP1) *player1Points -= 3;
        }
    }
}

int main(void)
{
    InitWindow(1366, 768, "Vertical Auto Sliding Cards");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));

    // Define main colors
    Color mainColors[MAX_LINES] = { RED, YELLOW, BLUE, BLACK, GREEN, BROWN };

    // Initialize cards
    Card cards[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++)
        cards[i].color = mainColors[rand() % MAX_LINES];

    // Collected cards counters per line
    int player1Count[MAX_LINES] = {0};
    int player2Count[MAX_LINES] = {0};

    float offsetY = 0.0f;
    float gameTimer = 0.0f;
    int playerTurn = 1;
    float turnTimer = 0.0f;
    int player1Points = 0, player2Points = 0;
    int blackShieldHolder = 0;

    const int sideMargin = 32;
    const int topMargin = 80;
    const int thumbW = CARD_WIDTH / 2;
    const int thumbH = CARD_HEIGHT / 2;
    const int thumbGap = GAP / 2;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        gameTimer += dt;
        turnTimer += dt;

        // Scroll carousel
        offsetY -= BASE_SPEED * dt;
        if (offsetY < -(CARD_HEIGHT + GAP)) {
            offsetY += (CARD_HEIGHT + GAP);
            Card temp = cards[0];
            for (int i = 0; i < MAX_CARDS - 1; i++)
                cards[i] = cards[i + 1];
            temp.color = mainColors[rand() % MAX_LINES];
            cards[MAX_CARDS - 1] = temp;
        }

        // Find the nearest card to center for highlight
        int selectedIndex = -1;
        float centerY = GetScreenHeight() / 2.0f - CARD_HEIGHT / 2.0f;
        float minDiff = 1e6f;
        for (int i = 0; i < MAX_CARDS; i++) {
            float y = offsetY + i * (CARD_HEIGHT + GAP) + 50;
            float diff = fabsf(y - centerY);
            if (diff < minDiff) { minDiff = diff; selectedIndex = i; }
        }

        // Player input
        if (selectedIndex != -1) {
            int pickPressed = 0;
            if (playerTurn == 1 && IsKeyPressed(KEY_SPACE)) pickPressed = 1;
            else if (playerTurn == 2 && IsKeyPressed(KEY_ENTER)) pickPressed = 1;

            if (pickPressed) {
                Card picked = cards[selectedIndex];
                int colorIndex = getColorIndex(picked.color, mainColors, MAX_LINES);
                if (colorIndex != -1) {
                    if (CompareColor(picked.color, BLACK))
                        blackShieldHolder = playerTurn;

                    if (playerTurn == 1 && player1Count[colorIndex] < MAX_BLOCKS * MAX_CARDS_PER_LINE)
                        player1Count[colorIndex]++;
                    else if (playerTurn == 2 && player2Count[colorIndex] < MAX_BLOCKS * MAX_CARDS_PER_LINE)
                        player2Count[colorIndex]++;

                    applyPoints(picked, playerTurn, &player1Points, &player2Points, blackShieldHolder);

                    // Switch turn after pick
                    playerTurn = (playerTurn == 1) ? 2 : 1;
                    turnTimer = 0.0f;
                    blackShieldHolder = 0;
                }
            }
        }

        // Auto-switch turn if 4 sec passed
        if (turnTimer >= TURN_TIME) {
            playerTurn = (playerTurn == 1) ? 2 : 1;
            turnTimer = 0.0f;
            blackShieldHolder = 0;
        }

        BeginDrawing();
        ClearBackground(GRAY);

        if (gameTimer < TOTAL_GAME_TIME) {
            // Draw central carousel
            for (int i = 0; i < MAX_CARDS; i++) {
                float x = GetScreenWidth() / 2 - CARD_WIDTH / 2;
                float y = offsetY + i * (CARD_HEIGHT + GAP) + 50;
                DrawRectangle(x, y, CARD_WIDTH, CARD_HEIGHT, cards[i].color);
                if (i == selectedIndex)
                    DrawRectangleLinesEx((Rectangle){x, y, CARD_WIDTH, CARD_HEIGHT}, 5, BLACK); // shading effect
            }

            // Draw collected cards per line
            for (int line = 0; line < MAX_LINES; line++) {
                // Player 1
                for (int i = 0; i < player1Count[line]; i++) {
                    int x = sideMargin + i * (thumbW + thumbGap);
                    int y = topMargin + line * (thumbH + thumbGap);
                    DrawRectangle(x, y, thumbW, thumbH, mainColors[line]);
                    DrawRectangleLinesEx((Rectangle){x, y, thumbW, thumbH}, 3, WHITE);
                }
                // Player 2
                for (int i = 0; i < player2Count[line]; i++) {
                    int x = GetScreenWidth() - sideMargin - ((i + 1) * thumbW + i * thumbGap);
                    int y = topMargin + line * (thumbH + thumbGap);
                    DrawRectangle(x, y, thumbW, thumbH, mainColors[line]);
                    DrawRectangleLinesEx((Rectangle){x, y, thumbW, thumbH}, 3, WHITE);
                }
            }

            DrawText("P1", sideMargin, 40, 28, WHITE);
            DrawText("P2", GetScreenWidth() - sideMargin - 28, 40, 28, WHITE);

            // Show current turn
            if (playerTurn == 1) DrawText("TURN", sideMargin, 70, 24, GREEN);
            else DrawText("TURN", GetScreenWidth() - sideMargin - 60, 70, 24, GREEN);

            // Timer and points info
            char info[128];
            int turnLeft = (int)(TURN_TIME - turnTimer); if (turnLeft < 0) turnLeft = 0;
            int totalLeft = (int)(TOTAL_GAME_TIME - gameTimer); if (totalLeft < 0) totalLeft = 0;
            snprintf(info, sizeof(info), "P1 Points: %d | P2 Points: %d | Turn: %d sec | Total: %d sec",
                     player1Points, player2Points, turnLeft, totalLeft);
            int textWidth = MeasureText(info, 28);
            DrawText(info, GetScreenWidth()/2 - textWidth/2, 10, 28, YELLOW);
        } else {
            // Game result
            const char *result;
            if (player1Points > player2Points) result = "Player 1 Wins!";
            else if (player2Points > player1Points) result = "Player 2 Wins!";
            else result = "Draw!";
            int w = MeasureText(result, 40);
            DrawText(result, GetScreenWidth()/2 - w/2, GetScreenHeight()/2 - 20, 40, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
