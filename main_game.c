#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARDS 10
#define GAP 10
#define MAX_ROW_CARDS 8 // per row

typedef enum
{
    CARD_NORMAL,
    CARD_HERO,
    CARD_WEATHER,
    CARD_LEADER,
    CARD_SPECIAL_UNIT
} CardType;

typedef enum
{
    ROW_MELEE,
    ROW_RANGED,
    ROW_SIEGE,
    ROW_GLOBAL // For weather cards
} RowType;

typedef struct
{
    CardType type;    // Unit / Spell / Hero / Weather
    RowType row;      // Where it can be played
    int basePower;    // Original power
    int currentPower; // Modified power after buffs/debuffs
    int isHero;       // Hero cards ignore weather
    int isGold;       // Special status (immune sometimes)
    Texture2D normTex;
    Texture2D rotatedTex; // Card artwork
} Card;

static Card CreateCard(CardType type, RowType row, int basePower, Image img, int hero, int gold)
{
    Card c;
    c.type = type;
    c.row = row;
    c.basePower = basePower;
    c.currentPower = basePower;
    c.isHero = hero;
    c.isGold = gold;

    // Normal texture
    c.normTex = LoadTextureFromImage(img);

    // Rotated texture
    Image temp = ImageCopy(img);
    ImageRotateCW(&temp);
    c.rotatedTex = LoadTextureFromImage(temp);
    UnloadImage(temp);

    return c;
}

int GetRandomCardIndex(Card *cards, int totalCards)
{
    int r = rand() % 100;
    CardType chosenType;
    if (r >= 0 && r <= 70)
        chosenType = CARD_NORMAL;
    else if (r > 70 && r <= 80)
        chosenType = CARD_WEATHER;
    else if (r > 80 && r <= 90)
        chosenType = CARD_SPECIAL_UNIT;
    else
        chosenType = CARD_HERO;

    int idx = -1;
    while (idx == -1)
    {
        int randIdx = rand() % totalCards;
        if (cards[randIdx].type == chosenType)
            idx = randIdx;
    }
    return idx;
}

int main(void)
{
    srand((unsigned)time(NULL));

    const int screenWidth = 1366;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "GOWTHER");
    InitAudioDevice();   // Initialize audio system

    // Load background music (must be a file like .mp3, .ogg, .wav)
    Music bgm = LoadMusicStream("dechire.mp3");

    // Play music
    PlayMusicStream(bgm);
    SetMusicVolume(bgm, 0.5f);  // optional: set volume to 50%
    SetTargetFPS(60);
    Card cards[12];

    // Load Images
    Image mugSold = LoadImage("mughalSoldier.png");
    Image boltu = LoadImage("bangabaltu.png");
    Image claw = LoadImage("clawarchi.png");
    Image fog = LoadImage("fog.png");
    Image frost = LoadImage("frostbite.png");
    Image hitler = LoadImage("hitler.png");
    Image khalid = LoadImage("khalid bin walid.png");
    Image mongarch = LoadImage("mongolArcher.png");
    Image odyss = LoadImage("odysseus.png");
    Image storm = LoadImage("storm.png");
    Image suleiman = LoadImage("suleiman.png");
    Image trebuchet = LoadImage("trebuchet.png");

    // Create Cards
    cards[0] = CreateCard(CARD_NORMAL, ROW_MELEE, 3, mugSold, 0, 0);
    cards[1] = CreateCard(CARD_SPECIAL_UNIT, ROW_MELEE, 3, boltu, 0, 1);
    cards[2] = CreateCard(CARD_HERO, ROW_SIEGE, 15, claw, 1, 0);
    cards[3] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, fog, 0, 2);
    cards[4] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, frost, 0, 1);
    cards[11] = CreateCard(CARD_LEADER, ROW_GLOBAL, 0, hitler, 0, 0);
    cards[6] = CreateCard(CARD_HERO, ROW_MELEE, 15, khalid, 1, 0);
    cards[7] = CreateCard(CARD_NORMAL, ROW_RANGED, 6, mongarch, 0, 0);
    cards[8] = CreateCard(CARD_HERO, ROW_RANGED, 15, odyss, 1, 0);
    cards[9] = CreateCard(CARD_WEATHER, ROW_GLOBAL, 0, storm, 0, 3);
    cards[10] = CreateCard(CARD_LEADER, ROW_GLOBAL, 0, suleiman, 0, 0);
    cards[5] = CreateCard(CARD_NORMAL, ROW_SIEGE, 8, trebuchet, 0, 0);

    // UI Textures
    Texture2D menuBG = LoadTexture("main menu.jpg");
    Texture2D gameBoard = LoadTexture("gameBoard.jpg");
    Texture2D upboard=LoadTexture("upboard.png");
    Texture2D downboard=LoadTexture("downboard.png");
    Texture2D buttons = LoadTexture("buttons.png");
    Texture2D timer = LoadTexture("time.png");
    Texture2D score = LoadTexture("score.png");
    Texture2D frostTex=LoadTexture("frost.jpg");

    Rectangle btnPlay = {200, 320, 200, 89};
    Rectangle btnQuit = {200, 390, 200, 89};

    enum GameState
    {
        MENU,
        PLAY,
        HELP,
        EXIT
    };
    int gameState = MENU;

    int queue[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++)
    {
        queue[i] = GetRandomCardIndex(cards, 10);
    }

    float offsetY = 0;
    const float BASE_SPEED = 100.0f; // px/sec
    const int CARD_HEIGHT = 135;
    const int CARD_WIDTH = 79;

    int rowCounts[3] = {0};                    // melee, ranged, siege counts
    int rs1[MAX_ROW_CARDS]; memset(rs1, -1, sizeof(rs1));
    int ra1[MAX_ROW_CARDS]; memset(ra1, -1, sizeof(ra1));
    int rsg1[MAX_ROW_CARDS]; memset(rsg1, -1, sizeof(rsg1));
    int frosto=0;
    int foggo=0;
    int stormo=0;

    int scores[3]={0};

    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }
        UpdateMusicStream(bgm);
        Vector2 mouse = GetMousePosition();

        // Input
        if (gameState == MENU)
        {
            if (CheckCollisionPointRec(mouse, btnPlay) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = PLAY;
            if (CheckCollisionPointRec(mouse, btnQuit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                gameState = EXIT;
        }
        if (gameState == EXIT)
            break;

        // Update
        if (gameState == PLAY)
        {
            float dt = GetFrameTime();

            // Scroll cards upwards
            offsetY -= BASE_SPEED * dt;
            if (offsetY < -(CARD_HEIGHT + GAP))
            {
                offsetY += (CARD_HEIGHT + GAP);

                // Shift queue up
                for (int i = 0; i < MAX_CARDS - 1; i++)
                    queue[i] = queue[i + 1];

                // Add new random card at the bottom
                queue[MAX_CARDS - 1] = GetRandomCardIndex(cards, 12);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground((Color){25, 25, 25, 255});

        if (gameState == MENU)
        {
            DrawTexture(menuBG, 0, 0, WHITE);
            DrawTexture(buttons, btnPlay.x, btnPlay.y, WHITE);
            DrawText("PLAY", btnPlay.x + 65, btnPlay.y + 42, 30,
                     CheckCollisionPointRec(mouse, btnPlay) ? YELLOW : WHITE);
            DrawTexture(buttons, btnQuit.x, btnQuit.y, WHITE);
            DrawText("Quit", btnQuit.x + 65, btnQuit.y + 42, 30,
                     CheckCollisionPointRec(mouse, btnQuit) ? YELLOW : WHITE);
        }
        else if (gameState == PLAY)
        {
            DrawTexture(gameBoard, 0, 0, WHITE);

            int selectedIndex = -1;

            // Selection zone in the middle
            Rectangle selectZone = {screenWidth / 2 - CARD_WIDTH / 2,
                                    screenHeight / 2 - CARD_HEIGHT / 2,
                                    CARD_WIDTH, CARD_HEIGHT};
            // Draw scrolling cards
            for (int i = 0; i < MAX_CARDS; i++)
            {
                float x = screenWidth / 2 - CARD_WIDTH / 2;
                float y = offsetY + i * (CARD_HEIGHT + GAP);
                Rectangle cardRect = {x, y, CARD_WIDTH, CARD_HEIGHT};

                DrawTexture(upboard,609,0,WHITE);
                DrawTexture(downboard,607,702,WHITE);
                DrawTexture(timer,618,0,BROWN);
                DrawTexture(timer,618,640,BROWN);
                DrawTexture(cards[queue[i]].normTex, x, y, WHITE);

                // Check if this card overlaps selection zone
                if (CheckCollisionRecs(selectZone, cardRect))
                {
                    selectedIndex = queue[i];
                }
            }

            // If Enter pressed, confirm card
            if (selectedIndex != -1 && IsKeyPressed(KEY_ENTER))
            {
                Card picked = cards[selectedIndex];

                if(picked.row == ROW_MELEE)
                {
                    if(rowCounts[2] <MAX_ROW_CARDS)
                    {
                        rs1[rowCounts[2]]=selectedIndex;
                        scores[2]+=cards[selectedIndex].currentPower;
                        rowCounts[2]++;
                    }
                }
                else if(picked.row == ROW_RANGED)
                {
                    if(rowCounts[1] <MAX_ROW_CARDS)
                    {
                        ra1[rowCounts[1]]=selectedIndex;
                        scores[1]+=cards[selectedIndex].currentPower;
                        rowCounts[1]++;
                    }
                }
                else if(picked.row == ROW_SIEGE)
                {
                    if(rowCounts[0] <MAX_ROW_CARDS)
                    {
                        rsg1[rowCounts[0]]=selectedIndex;
                        scores[0]+=cards[selectedIndex].currentPower;
                        rowCounts[0]++;
                    }
                }
                else if(picked.row == ROW_GLOBAL)
                {
                    if(picked.type == CARD_WEATHER)
                    {
                        if(picked.isGold == 1)
                        {
                            frosto=1;
                        }
                    }
                }
            }

            for(int i=0;i<rowCounts[2];i++)
            {
                int cardIdx=rs1[i];
                int x=463;
                int y=65+4+i*79;
                DrawTexture(cards[cardIdx].rotatedTex, x, y, WHITE);
            }
            for(int i=0;i<rowCounts[1];i++)
            {
                int cardIdx=ra1[i];
                int x=311;
                int y=65+4+i*79;
                DrawTexture(cards[cardIdx].rotatedTex, x, y, WHITE);
            }
            for(int i=0;i<rowCounts[0];i++)
            {
                int cardIdx=rsg1[i];
                int x=159;
                int y=65+4+i*79;
                DrawTexture(cards[cardIdx].rotatedTex, x, y, WHITE);
            }
            if(frosto == 1)
            {
                DrawTexture(frostTex, 464, 76, Fade(WHITE,0.7f));
                DrawTexture(frostTex, 766, 76, Fade(WHITE,0.7f));
            }
            DrawTexture(score,494,681,BROWN);
            DrawTexture(score,342,681,BROWN);
            DrawTexture(score,190,681,BROWN);
            DrawTexture(score,805,681,BROWN);
            DrawTexture(score,957,681,BROWN);
            DrawTexture(score,1102,681,BROWN);
            DrawText(TextFormat("%d", scores[2]), 517, 700, 30, WHITE);
            DrawText(TextFormat("%d", scores[1]), 365, 700, 30, WHITE);
            DrawText(TextFormat("%d", scores[0]), 213, 700, 30, WHITE);
        }

        EndDrawing();
    }

    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}