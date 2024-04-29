#include "raylib.h"
#include <stdlib.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

typedef struct Player {
    Vector2 position;
    int size;
    Color color;
} Player;

typedef struct World {
    Vector2 units;
    int size;
    int* map;
} World;

Player player;
World world;

void InitGame(void);
void UpdateControls(void);
void UpdateGame(void);
void DrawPlayer(void);
void Draw2DMap(void);
void DrawGame(void);
void DeInitGame(void);

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mode7 Demo");
    SetTargetFPS(60);

    InitGame();

    while (!WindowShouldClose()) {
        UpdateGame();

        BeginDrawing();
        DrawGame();
        EndDrawing();
    }

    DeInitGame();
    CloseWindow();

    return 0;
}

void InitGame(void) {
    player.position.x = 300;
    player.position.y = 300;
    player.size = 8;
    player.color = YELLOW;

    world.units.x = 8;
    world.units.y = 8;
    world.size = 64;
    world.map = (int*) malloc(world.units.x * world.units.y * sizeof(int));

    // replace with generated map later
    int temp_map[8][8] = {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            world.map[y * 8 + x] = temp_map[y][x];
        }
    }
}

void UpdateControls(void) {
    float *px = &player.position.x;
    float *py = &player.position.y;

    if (IsKeyDown(KEY_A)) *px-=5;
    if (IsKeyDown(KEY_D)) *px+=5;
    if (IsKeyDown(KEY_W)) *py-=5;
    if (IsKeyDown(KEY_S)) *py+=5;
}

void UpdateGame(void) {
    UpdateControls();
}

void DrawPlayer(void) {
    int startX = player.position.x;
    int startY = player.position.y;
    int size = player.size;
    Color color = player.color;
    DrawRectangle(startX, startY, size, size, color);
}

void Draw2DMap(void) {
    Color color;
    for (int y = 0; y < world.units.y; y++) {
        for (int x = 0; x < world.units.x; x++) {
            if (world.map[(int)(y * world.units.x + x)] == 1) {
                color = RAYWHITE;
            } else {
                color = BLACK;
            }
            int x0 = x * world.size;
            int y0 = y * world.size;
            DrawRectangle(x0+1, y0+1, world.size-1, world.size-1, color);
        }
    }
}

void DrawGame(void) {
    ClearBackground(DARKGRAY);
    DrawFPS(10, 10);

    Draw2DMap();
    DrawPlayer();
}

void DeInitGame(void) {
    free(world.map);
}