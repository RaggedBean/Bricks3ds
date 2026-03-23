#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>

#include "player.h"
#include "ball.h"
#include "brick.h"
#include "bricks.h"

using namespace std;

#define TOP_WIDTH  400
#define TOP_HEIGHT 240
#define BOT_WIDTH  320
#define BOT_HEIGHT 240

void resetBall(struct Ball* ball, struct Player* player);
bool checkCollisions(struct Ball* ball, struct Brick* brick);
void initBricks(Bricks* bricks);
void WinScreen(C3D_RenderTarget* screen);
void gameOver(C3D_RenderTarget* screen);
void loadLevelFromFile(Bricks* bricks, const char* path);
void loadCurrentLevel(Bricks* bricks, int levelIndex, const std::vector<std::string>& levels);

int main(int argc, char* argv[])
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Romfs Init
    romfsInit();

    FILE* file = fopen("romfs:/levels/level1.txt", "r");
    if (!file) {
        printf("Erreur d'ouverture du fichier\n");
        return -1;
    }

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    u32 clrRed = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
    u32 clrOrange = C2D_Color32(0xFF, 0xA5, 0x00, 0xFF);
    u32 clrYellow = C2D_Color32(0xFF, 0xFF, 0x00, 0xFF);
    u32 clrGreen = C2D_Color32(0x00, 0x80, 0x00, 0xFF);
    u32 clrBlue = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
    u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);

    u32 clrClear = C2D_Color32(0x10, 0x10, 0x10, 0xFF);
    u32 clrBall = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    u32 clrPad = C2D_Color32(0xC0, 0xC0, 0xC0, 0xFF);

    vector<string> levels = {
        "romfs:/levels/level1.txt",
        "romfs:/levels/level2.txt",
        "romfs:/levels/level3.txt"
    };

    int currentLevel = 0;

    bool isGameOver = false;
    bool isGameCleared = false;

    struct Player player = {BOT_WIDTH / 2, BOT_HEIGHT - 20, 50, 10, 200, 3};
    struct Ball ball = {player.x + player.w, BOT_HEIGHT + player.y - 10, 0, 0, 10, 10, 0, 0, false};
    struct Bricks bricks = {15, 12, 25.f, 15.f, 32, 15.f, 0, 0, {}};

    float total_width = bricks.col * bricks.w + (bricks.col - 1) * bricks.padding_w;
    bricks.top_left_pos_x = (TOP_WIDTH - total_width) / 2.0f;

    loadCurrentLevel(&bricks, currentLevel, levels);


    u64 prevTime = osGetTime();

    while (aptMainLoop())
    {
        hidScanInput();
        gspWaitForVBlank();

        u64 currTime = osGetTime();
        float dt = (currTime - prevTime) / 1000.0f;
        prevTime = currTime;

        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        if (kDown & KEY_START)
            break;

        if (isGameCleared) {
            WinScreen(top);
            if (kDown & KEY_A) {
                player.lifes = 3;
                player.x = BOT_WIDTH / 2;
                ball = {player.x + player.w, BOT_HEIGHT + player.y - 10, 0, 0, 10, 10, 0, 0, false};
                resetBall(&ball, &player);
                currentLevel = 0;
                bricks.current_level_bricks.clear();
                loadLevelFromFile(&bricks, levels[currentLevel].c_str());
                isGameCleared = false;
            }
            C3D_FrameEnd(0);
            continue;
        }           

        if (isGameOver) {
            gameOver(top);
            if (kDown & KEY_A) {
                player.lifes = 3;
                player.x = BOT_WIDTH / 2;
                ball = {player.x + player.w, BOT_HEIGHT + player.y - 10, 0, 0, 10, 10, 0, 0, false};
                resetBall(&ball, &player);
                bricks.current_level_bricks.clear();
                currentLevel = 0;
                loadCurrentLevel(&bricks, currentLevel, levels);
                isGameOver = false;
            }
            
            C3D_FrameEnd(0);
            continue;
        }

        if (ball.isPlaying) {
            ball.x += ball.speed_x * dt;
            ball.y += ball.speed_y * dt;
        }

        if (ball.x <= 0 || ball.x + ball.w >= TOP_WIDTH)
            ball.speed_x *= -1;

        if (ball.y <= 0)
            ball.speed_y *= -1;

        if (ball.y >= TOP_HEIGHT) {
            ball.bot_y = ball.y - TOP_HEIGHT;
            ball.bot_x = ball.x * (BOT_WIDTH / (float)TOP_WIDTH);
            if (ball.bot_y + ball.h >= player.y && ball.bot_y + ball.h <= player.y + player.h &&
                ball.bot_x + ball.w >= player.x && ball.bot_x <= player.x + player.w) {
                float relativePos = ((ball.bot_x + ball.w / 2) - player.x) / player.w;
                ball.speed_y *= -1;
                ball.speed_x = (relativePos - 0.7f) * 2 * 200.0f;
            }
        }

        if (bricks.current_level_bricks.empty()) {
            currentLevel++;
            if (currentLevel < levels.size()) {
                loadCurrentLevel(&bricks, currentLevel, levels);
                resetBall(&ball, &player);
            } else {
                isGameCleared = true;
            }
        }        

        if (ball.y > TOP_HEIGHT + BOT_HEIGHT) {
            player.lifes--;
            resetBall(&ball, &player);
        }

        if (player.lifes <= 0) {
            currentLevel = 0;
            isGameOver = true;
        }

        if (player.x <= 0) player.x = 0;
        if (player.x + player.w >= BOT_WIDTH) player.x = BOT_WIDTH - player.w;

        if (!ball.isPlaying) {
            ball.bot_x = player.x + player.w / 2 - ball.w / 2;
            ball.bot_y = player.y - ball.h;
            ball.x = ball.bot_x * (TOP_WIDTH / (float)BOT_WIDTH);
            ball.y = TOP_HEIGHT + ball.bot_y;
        }

        if (kDown & KEY_A && !ball.isPlaying) {
            ball.isPlaying = true;
            ball.speed_x = -170;
            ball.speed_y = -170;
        }

        if (kHeld & KEY_CSTICK_LEFT || kHeld & KEY_LEFT)
            player.x -= player.speed * dt;
        if (kHeld & KEY_CSTICK_RIGHT || kHeld & KEY_RIGHT)
            player.x += player.speed * dt;

        for (auto it = bricks.current_level_bricks.begin(); it != bricks.current_level_bricks.end(); ) {
            if (checkCollisions(&ball, &(*it))) {
                it->pv -= 1;
                if (it->pv == 2) {
                    it->color = clrBlue;
                }
                if (it->pv == 1) {
                    it->color = clrGreen;
                }
                if (it->pv <= 0) {
                    it = bricks.current_level_bricks.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }

        // DRAW

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        C2D_TargetClear(top, clrClear);
        C2D_SceneBegin(top);

        if (ball.y < TOP_HEIGHT)
            C2D_DrawRectangle(ball.x, ball.y, 0, ball.w, ball.h, clrBall, clrBall, clrBall, clrBall);

        for (auto& brick : bricks.current_level_bricks)
            brick.draw();

        C2D_TargetClear(bottom, clrClear);
        C2D_SceneBegin(bottom);

        player.draw();

        if (ball.y >= TOP_HEIGHT)
            C2D_DrawRectangle(ball.x * (BOT_WIDTH / (float)TOP_WIDTH), ball.y - TOP_HEIGHT,
                              0, ball.w, ball.h, clrBall, clrBall, clrBall, clrBall);

        char lifes_text[32];
        snprintf(lifes_text, sizeof(lifes_text), "LIFES: %d", player.lifes);
        C2D_TextBuf textBuf = C2D_TextBufNew(64);
        C2D_Text text;
        C2D_TextParse(&text, textBuf, lifes_text);
        C2D_TextOptimize(&text);
        C2D_DrawText(&text, C2D_WithColor, 10.0f, 10.0f, 0.5f, 1.0f, 1.0f, clrWhite);
        C2D_TextBufDelete(textBuf);

        C3D_FrameEnd(0);
    }

    fclose(file);
    romfsExit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}

void resetBall(struct Ball* ball, struct Player* player)
{
    ball->isPlaying = false;
    ball->speed_x = 0;
    ball->speed_y = 0;
    ball->bot_x = player->x + player->w / 2 - ball->w / 2;
    ball->bot_y = player->y - ball->h;
    ball->x = ball->bot_x * (TOP_WIDTH / (float)BOT_WIDTH);
    ball->y = TOP_HEIGHT + ball->bot_y;
}

bool checkCollisions(struct Ball* ball, struct Brick* brick)
{
    if (ball->x + ball->w > brick->x && ball->x < brick->x + brick->w &&
        ball->y + ball->h > brick->y && ball->y < brick->y + brick->h) {

        float overlapX = min(ball->x + ball->w, brick->x + brick->w) - max(ball->x, brick->x);
        float overlapY = min(ball->y + ball->h, brick->y + brick->h) - max(ball->y, brick->y);

        if (overlapX < overlapY)
            ball->speed_x *= -1;
        else
            ball->speed_y *= -1;

        return true;
    }
    return false;
}

void initBricks(Bricks* bricks) {
    for (size_t r = 1; r < bricks->row - 1; r++) {
        for (size_t c = 1; c < bricks->col - 1; c++) {
            float curr_x = bricks->top_left_pos_x + (c - 1) * (bricks->w + bricks->padding_w);
            float curr_y = bricks->top_left_pos_y + (r - 1) * (bricks->h + bricks->padding_h);
            Brick b = {curr_x, curr_y, bricks->w, bricks->h};
            bricks->current_level_bricks.push_back(b);
        }
    }
}

void gameOver(C3D_RenderTarget* screen) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(screen, C2D_Color32(0x10, 0x10, 0x10, 0xFF));
    C2D_SceneBegin(screen);

    C2D_TextBuf textBuf = C2D_TextBufNew(64);
    C2D_Text text;
    C2D_TextParse(&text, textBuf, "GAME OVER");
    C2D_TextOptimize(&text);

    float textWidth, textHeight;
    C2D_TextGetDimensions(&text, 1.5f, 1.5f, &textWidth, &textHeight);
    float x = (TOP_WIDTH - textWidth) / 2.0f;
    float y = (TOP_HEIGHT - textHeight) / 2.0f;

    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, 1.5f, 1.5f, C2D_Color32(255, 0, 0, 255));
    C2D_TextBufDelete(textBuf);
    C3D_FrameEnd(0);
}

void WinScreen(C3D_RenderTarget* screen) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(screen, C2D_Color32(0x10, 0x10, 0x10, 0xFF));
    C2D_SceneBegin(screen);

    C2D_TextBuf textBuf = C2D_TextBufNew(64);
    C2D_Text text;
    C2D_TextParse(&text, textBuf, "YOU WIN !");
    C2D_TextOptimize(&text);

    float textWidth, textHeight;
    C2D_TextGetDimensions(&text, 1.5f, 1.5f, &textWidth, &textHeight);
    float x = (TOP_WIDTH - textWidth) / 2.0f;
    float y = (TOP_HEIGHT - textHeight) / 2.0f;

    C2D_DrawText(&text, C2D_WithColor, x, y, 0.5f, 1.5f, 1.5f, C2D_Color32(0x00, 0x00, 0xFF, 0xFF));
    C2D_TextBufDelete(textBuf);
    C3D_FrameEnd(0);
}


void loadLevelFromFile(Bricks* bricks, const char* path) {
    std::ifstream file(path);
    if (!file) {
        printf("Erreur ouverture fichier niveau : %s\n", path);
        return;
    }

    float total_w = bricks->col * (bricks->w + bricks->padding_w);
    float total_h = bricks->row * (bricks->h + bricks->padding_h);
    bricks->top_left_pos_x = (400 - total_w) / 2.f;
    bricks->top_left_pos_y = (240 - total_h) / 2.f;

    bricks->current_level_bricks.clear();

    std::string line;
    size_t row = 0;
    while (std::getline(file, line) && row < bricks->row) {
        for (size_t col = 0; col < line.size() && col < bricks->col; ++col) {
            if (line[col] != '0') {
                float x = bricks->top_left_pos_x + col * (bricks->w + bricks->padding_w);
                float y = bricks->top_left_pos_y + row * (bricks->h + bricks->padding_h);
                
                Brick b = {x, y, bricks->w, bricks->h};
                
                if (line[col] == '1') {
                    b.pv = 1;
                    b.color = C2D_Color32(0x00, 0x80, 0x00, 0xFF);
                }
                if (line[col] == '2') {
                    b.pv = 2;
                    b.color = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
                }
                if (line[col] == '3') {
                    b.pv = 3;
                    b.color = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
                }
                bricks->current_level_bricks.push_back(b);
            }
        }
        row++;
    }
}


void loadCurrentLevel(Bricks* bricks, int levelIndex, const std::vector<std::string>& levels) {
    if (levelIndex < levels.size()) {
        bricks->current_level_bricks.clear();
        loadLevelFromFile(bricks, levels[levelIndex].c_str());
    } else {
        // Dernier niveau fini => Game clear
        bricks->current_level_bricks.clear();
    }
}
