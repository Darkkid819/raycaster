#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

#define DR 0.0174533 // 1 degree in radians

typedef struct Player {
    Vector2 position;
    Vector2 delta;
    float angle;
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
float dist(float, float, float, float, float);
void DrawRays2D(void);
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
    player.position.x = 300.0f;
    player.position.y = 300.0f;
    player.angle = 1.0f;
    player.delta.x = cos(player.angle) * 5.0f;
    player.delta.y = sin(player.angle) * 5.0f;
    player.size = 8;
    player.color = YELLOW;
    world.units.x = 8.0f;
    world.units.y = 8.0f;
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
    float* px = &player.position.x;
    float* py = &player.position.y;
    float* pdx = &player.delta.x;
    float* pdy = &player.delta.y;
    float* pa = &player.angle;

    if (IsKeyDown(KEY_A)) {
        *pa-=0.1;
        if (*pa < 0) {
            *pa += 2*PI;
        }
        *pdx = cos(*pa) * 5;
        *pdy = sin(*pa) * 5;
    } 
    if (IsKeyDown(KEY_D)) {
        *pa+=0.1;
        if (*pa > 2 * PI) {
            *pa -= 2*PI;
        }
        *pdx = cos(*pa) * 5;
        *pdy = sin(*pa) * 5;
    }
    if (IsKeyDown(KEY_W)) {
        *px += *pdx;
        *py += *pdy;
    }
    if (IsKeyDown(KEY_S)) {
        *px -= *pdx;
        *py -= *pdy;
    }
}

void UpdateGame(void) {
    UpdateControls();
}

void DrawPlayer(void) {
    int startX = player.position.x;
    int startY = player.position.y;
    int endX = startX + player.delta.x * 5;
    int endY = startY + player.delta.y * 5;
    
    Vector2 start = { (float)startX, (float)startY };
    Vector2 end = { (float)endX, (float)endY };
    float thickness = 3.0f;
    Color color = YELLOW;

    DrawLineEx(start, end, thickness, color); 

    DrawRectangle(startX - player.size / 2, startY - player.size / 2, player.size, player.size, player.color);
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

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

void DrawRays2D(void) {
    int r, mx, my, mp, dof;
    float rx, ry, ra, x0, y0;
    ra = player.angle - DR * 30;
    if (ra < 0) {
        ra += 2 * PI;
    }
    if (ra > 2 * PI) {
        ra -= 2 * PI;
    }
    float px = player.position.x;
    float py = player.position.y;
    int mapX = world.units.x;
    int mapY = world.units.y;

    for (r = 0; r < 60; r++) {
        dof = 0;
        float disH = 1000000;
        float hx = px;
        float hy = py;
        float aTan = -1/tan(ra);

        // check horizontal lines
        if (ra > PI) { // looking up
            ry = (((int) py >> 6) << 6) - 0.0001f; 
            rx = (py - ry) * aTan + px;
            y0 = -64.0f;
            x0 = -y0 * aTan;
        }
        if (ra < PI) { // looking down
            ry = (((int) py >> 6) << 6) + 64.0f; 
            rx = (py - ry) * aTan + px;
            y0 = 64.0f;
            x0 = -y0 * aTan;
        }
        if (ra == 0 || ra == 2 * PI) { // left or right
            rx = px; 
            ry = py; 
            dof = 8;
        }
        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && world.map[mp] == 1) { // hit wall
                hx = rx;
                hy = ry;
                disH = dist(px, py, hx, hy, ra);
                dof = 8; 
            } else {
                rx += x0;
                ry += y0;
                dof += 1;
            }
        }

        float disV = 1000000;
        float vx = px;
        float vy = py;
        dof = 0;
        float nTan = -tan(ra);

        // check vertical lines
        if (ra > PI/2 && ra < 3*PI/2) { // left
            rx = (((int) px >> 6) << 6) - 0.0001f; 
            ry = (px - rx) * nTan + py;
            x0 = -64.0f;
            y0 = -x0 * nTan;
        }
        if (ra < PI/2 || ra > 3*PI/2) { // right
            rx = (((int) px >> 6) << 6) + 64.0f; 
            ry = (px - rx) * nTan + py;
            x0 = 64.0f;
            y0 = -x0 * nTan;
        }
        if (ra == PI/2 || ra == 3*PI/2) { // up or down
            rx = px; 
            ry = py; 
            dof = 8;
        }
        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && world.map[mp] == 1) { // hit wall
                vx = rx;
                vy = ry;
                disV = dist(px, py, vx, vy, ra);
                dof = 8; 
            } else {
                rx += x0;
                ry += y0;
                dof += 1;
            }
        }

        if (disV < disH) {
            rx = vx;
            ry = vy;
        }
        if (disH < disV) {
            rx = hx;
            ry = hy;
        }

        DrawLineEx((Vector2){px, py}, (Vector2){rx, ry}, 3, RED);

        ra += DR;
        if (ra < 0) {
            ra += 2 * PI;
        }
        if (ra > 2 * PI) {
            ra -= 2 * PI;
        }
    }
}

void DrawGame(void) {
    ClearBackground(DARKGRAY);
    DrawFPS(10, 10);

    Draw2DMap();
    DrawRays2D();
    DrawPlayer();
}

void DeInitGame(void) {
    free(world.map);
}