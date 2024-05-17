#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

//-----------------------------MAP----------------------------------------------
#define MAP_X 8
#define MAP_Y 8
#define MAP_SIZE 64

int map[] =
{
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,1,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};

void Draw2DMap() {
    Color color;
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            color = (map[y * MAP_X + x] == 1) ? WHITE : BLACK;
            int x0 = x * MAP_SIZE;
            int y0 = y * MAP_SIZE;
            DrawRectangle(x0+1, y0+1, MAP_SIZE-2, MAP_SIZE-2, color);
        }
    }
}

//------------------------PLAYER------------------------------------------------
float degToRad(float a) { 
    return a * PI / 180.0f;
}

int FixAng(float a){ 
    if(a > 359.0f){ 
        a -= 360.0f;
    } 
    if(a<0){ 
        a += 360.0f;
    } 
    return a;
}

float px,py,pdx,pdy,pa;

void DrawPlayer() {
    DrawCircle(px, py, 4, YELLOW);
    DrawLine(px, py, px + pdx * 20, py + pdy * 20, YELLOW);
}

void controls() {
    if (IsKeyDown(KEY_A)) {
        pa += 0.05 * GetFPS();
        pa = FixAng(pa);
        pdx = cos(degToRad(pa));
        pdy = -sin(degToRad(pa));
    } 
    if (IsKeyDown(KEY_D)) {
        pa -= 0.05 * GetFPS();
        pa = FixAng(pa);
        pdx = cos(degToRad(pa));
        pdy = -sin(degToRad(pa));
    }
    if (IsKeyDown(KEY_W)) {
        px += pdx * 0.08 * GetFPS();
        py += pdy * 0.08 * GetFPS();
    }
    if (IsKeyDown(KEY_S)) {
        px -= pdx * 0.08 * GetFPS();
        py -= pdy * 0.08 * GetFPS();
    }
}

//---------------------------Draw Rays and Walls--------------------------------
float dist(float ax, float ay, float bx, float by, float ang) {
    return cos(degToRad(ang)) * (bx - ax) - sin(degToRad(ang)) * (by - ay);
}

void DrawRays2D() {
    DrawRectangle(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight() / 2, SKYBLUE);
    DrawRectangle(GetScreenWidth() / 2, GetScreenHeight() / 2, GetScreenWidth() / 2, GetScreenHeight() / 2, DARKBLUE);

    int r, mx, my, mp, dof;
    float vx, vy, rx, ry, ra, xo, yo, disV, disH, Tan;
    ra = FixAng(pa + 30.0f);

    for (r = 0; r < 60; r++) {
        //---Vertical---
        dof = 0; 
        // side = 0; 
        disV = 100000.0f;
        Tan = tan(degToRad(ra));

        if (cos(degToRad(ra)) > 0.001f) { // Looking left
            rx = (((int)px >> 6) << 6) + 64.0f;
            ry = (px - rx) * Tan + py;
            xo = 64.0f;
            yo = -xo * Tan;
        } else if (cos(degToRad(ra)) < -0.001f) { // Looking right
            rx = (((int)px >> 6) << 6) - 0.0001f;
            ry = (px - rx) * Tan + py;
            xo = -64.0f;
            yo = -xo * Tan;
        } else {
            rx = px;
            ry = py;
            dof = 8; // Looking up or down, no horizontal check
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * MAP_X + mx;
            if (mp > 0 && mp < MAP_X * MAP_Y && map[mp] == 1) {
                dof = 8; // Hit
                disV = dist(px, py, rx, ry, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1; // Check next vertical
            }
        }
        vx = rx; 
        vy = ry;

        //---Horizontal---
        dof = 0; 
        disH = 100000.0f;
        Tan = 1.0 / Tan;

        if (sin(degToRad(ra)) > 0.001f) { // Looking up
            ry = (((int)py >> 6) << 6) - 0.0001f;
            rx = (py - ry) * Tan + px;
            yo = -64.0f;
            xo = -yo * Tan;
        } else if (sin(degToRad(ra)) < -0.001f) { // Looking down
            ry = (((int)py >> 6) << 6) + 64.0f;
            rx = (py - ry) * Tan + px;
            yo = 64.0f;
            xo = -yo * Tan;
        } else {
            rx = px;
            ry = py;
            dof = 8; // Looking straight left or right
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * MAP_X + mx;
            if (mp > 0 && mp < MAP_X * MAP_Y && map[mp] == 1) {
                dof = 8; // Hit
                disH = dist(px, py, rx, ry, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1; // Check next horizontal
            }
        }

        Color rayColor = LIME;
        if (disV < disH) {
            rx = vx;
            ry = vy;
            disH = disV;
            rayColor = GREEN;
        }
        DrawLine(px, py, rx, ry, rayColor); // 2D ray

        int ca = FixAng(pa - ra);
        disH = disH * cos(degToRad(ca)); // Fix fisheye
        int lineH = (MAP_SIZE * GetScreenHeight()) / disH;
        if (lineH > GetScreenHeight()) lineH = GetScreenHeight();
        int lineOff = (GetScreenWidth() / 4) - (lineH >> 1);

        DrawRectangle(r * 9 + GetScreenWidth() / 2, lineOff, 9, lineH, rayColor);

        ra = FixAng(ra - 1);
    }
}

//------------------------GAME--------------------------------------------------

void InitGame() {
    px=150; 
    py=400; 
    pa=90;
    pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); 
}

void UpdateGame(void) {
    controls();
}

void DrawGame(void) {
    ClearBackground(DARKGRAY);

    Draw2DMap();
    DrawPlayer();
    DrawRays2D();

    DrawFPS(10, 10);
}

void DeInitGame(void) {
    // TODO
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster Demo");
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