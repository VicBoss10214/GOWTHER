#include "raylib.h"
#include <time.h>
#include <stdlib.h>

typedef enum {
    CARD_NORMAL,
    CARD_SPELL,
    CARD_HERO,
    CARD_WEATHER,
    CARD_LEADER,
    CARD_SPECIAL_UNIT
} CardType;

typedef enum {
    ROW_MELEE,
    ROW_RANGED,
    ROW_SIEGE,
    ROW_GLOBAL // For weather cards
} RowType;

typedef struct {
    CardType type;       // Unit / Spell / Hero / Weather
    RowType row;         // Where it can be played
    int basePower;       // Original power
    int currentPower;    // Modified power after buffs/debuffs
    int isHero;          // Hero cards ignore weather
    int isGold;          // Special status (immune sometimes)
    Image image;
    Texture2D normTex;
    Texture2D rotatedTex;   // Card artwork
} Card;

Card CreateCard(CardType type, RowType row, int basePower, Image img, int hero, int gold) {
    Card c;
    c.type = type;
    c.row = row;
    c.basePower = basePower;
    c.currentPower = basePower;
    c.image = img;
    c.normTex = LoadTextureFromImage(c.image);
    ImageRotateCW(&c.image);
    c.rotatedTex = LoadTextureFromImage(c.image);
    c.isHero = hero;
    c.isGold = gold;
    return c;
}

Card GetRandomCard(Card *cards, int totalCards) {
    int r = rand() % 100; // random number 0–99

    CardType chosenType;
    if (r < 70) chosenType = CARD_NORMAL;        // 70%
    else if (r < 90) chosenType = CARD_SPECIAL_UNIT; // 20%
    else chosenType = CARD_HERO;                 // 10%

    // Now select a random card of that type
    Card selected;
    int found = 0;

    while (!found) {
        int idx = rand() % totalCards;
        if (cards[idx].type == chosenType) {
            selected = cards[idx];
            found = 1;
        }
    }

    return selected;
}

int main(void) {
    srand(time(NULL));
    const int screenWidth = 1366;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "GOWTHER");
    SetTargetFPS(60);

    Card cards[12];

    // Load Images
    Image mugSold = LoadImage("mughalSoldier.png");
    Image boltu = LoadImage("bangabaltu.png");
    Image claw = LoadImage("clawarchi.png");
    Image fog = LoadImage("fog.png");
    Image frost = LoadImage("frostbite.jpg");
    Image hitler = LoadImage("hitler.png");
    Image khalid = LoadImage("khalid.png");
    Image mongarch = LoadImage("mongolArcher.png");
    Image odyss = LoadImage("odysseus.png");
    Image storm = LoadImage("storm.png");
    Image suleiman = LoadImage("suleiman.png");
    Image trebuchet = LoadImage("trebuchet.png");

    // Create Cards
    cards[0] = CreateCard(CARD_NORMAL, ROW_MELEE, 5, mugSold, 0, 0);
    cards[1] = CreateCard(CARD_SPECIAL_UNIT, ROW_MELEE, 3, boltu, 0, 1);
    cards[2] = CreateCard(CARD_HERO, ROW_SIEGE, 15, claw, 1, 0);
    cards[3] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, fog, 0, 0);
    cards[4] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, frost, 0, 0);
    cards[5] = CreateCard(CARD_LEADER, ROW_GLOBAL, 0, hitler, 0, 0);
    cards[6] = CreateCard(CARD_HERO, ROW_MELEE, 15, khalid, 1, 0);
    cards[7] = CreateCard(CARD_NORMAL, ROW_RANGED, 6, mongarch, 0, 0);
    cards[8] = CreateCard(CARD_HERO, ROW_RANGED, 15, odyss, 1, 0);
    cards[9] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, storm, 0, 0);
    cards[10] = CreateCard(CARD_LEADER, ROW_GLOBAL, 0, suleiman, 0, 0);
    cards[11] = CreateCard(CARD_NORMAL, ROW_SIEGE, 8, trebuchet, 0, 0);

    // Load Textures for UI
    Texture2D menuBG = LoadTexture("main menu.jpg");
    Texture2D gameBoard = LoadTexture("gameBoard.jpg");
    Texture2D buttons = LoadTexture("buttons.png");

    Rectangle btnPlay = { 200, 320, 200, 89 };
    Rectangle btnQuit = { 200, 390, 200, 89 };

    enum GameState { MENU, PLAY, HELP, EXIT };
    int gameState = MENU;

    // Fill cards1 array with random cards
    Card cards1[1000];
    for(int i = 0; i < 1000; i++) {
        cards1[i] = GetRandomCard(cards, 12);
    }

    // Scrolling variables
    float offsetY = 0;
    const float BASE_SPEED = 100.0f;  // pixels per second
    const int CARD_HEIGHT = 135;
    const int CARD_WIDTH = 79;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        float dt = GetFrameTime();  // time between frames

        // Handle menu logic
        if (gameState == MENU) {
            if (CheckCollisionPointRec(mouse, btnPlay) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = PLAY;
            if (CheckCollisionPointRec(mouse, btnQuit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = EXIT;
        }
        if (gameState == EXIT) break;

        // Update scrolling
        if (gameState == MENU) {
            offsetY += BASE_SPEED * dt;
            if (offsetY > CARD_HEIGHT) offsetY -= CARD_HEIGHT;
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color){25, 25, 25, 255}); // dark background

        if (gameState == MENU) {
            DrawTexture(menuBG, 0, 0, WHITE);
            DrawTexture(buttons, btnPlay.x, btnPlay.y, WHITE);
            DrawText("PLAY", btnPlay.x + 65, btnPlay.y + 42, 30, CheckCollisionPointRec(mouse, btnPlay) ? YELLOW : WHITE);
            DrawTexture(buttons, btnQuit.x, btnQuit.y, WHITE);
            DrawText("Quit", btnQuit.x + 65, btnQuit.y + 42, 30, CheckCollisionPointRec(mouse, btnQuit) ? YELLOW : WHITE);

            // Draw scrolling cards in the middle column
            int centerX = screenWidth / 2 - CARD_WIDTH / 2;
            for (int i = 0; i < 1000; i++) {
                float y = i * CARD_HEIGHT - offsetY;
                if (y + CARD_HEIGHT > 0 && y < screenHeight) {
                    DrawTexture(cards1[i].normTex, centerX, y, WHITE);
                }
            }
        }
        else if (gameState == PLAY) {
            DrawTexture(gameBoard, 0, 0, WHITE);
            DrawText("GAME STARTED!", screenWidth/2 - MeasureText("GAME STARTED!", 40)/2, 250, 40, RAYWHITE);
            DrawText("Press ESC to return", screenWidth/2 - MeasureText("Press ESC to return", 20)/2, 300, 20, RAYWHITE);
            DrawTexture(cards[0].rotatedTex, 0, 0, WHITE); // example card
            if (IsKeyPressed(KEY_BACKSPACE)) gameState = MENU;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
