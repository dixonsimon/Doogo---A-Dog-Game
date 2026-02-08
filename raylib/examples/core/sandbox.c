#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

#define WORLD_SIZE 16
#define BLOCK_SIZE 2.0f

typedef enum BlockType {
    BLOCK_AIR = 0,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_STONE,
    BLOCK_WATER,
    BLOCK_COUNT
} BlockType;

typedef struct Block {
    BlockType type;
    bool active;
} Block;

// Function declarations
void InitWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE]);
void UpdateWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE], Camera3D camera);
void DrawWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE]);
Color GetBlockColor(BlockType type);
Vector3 GetBlockPosition(int x, int y, int z);

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "3D Sandbox Game");
    
    // Define the camera to look into our 3D world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ WORLD_SIZE*BLOCK_SIZE/2, WORLD_SIZE*BLOCK_SIZE/2, WORLD_SIZE*BLOCK_SIZE/2 };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SetCameraMode(camera, CAMERA_FREE); // Set a free camera mode
    
    // Initialize the world
    Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];
    InitWorld(world);
    
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        UpdateCamera(&camera);
        UpdateWorld(world, camera);
        
        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(camera);
                DrawWorld(world);
                DrawGrid(WORLD_SIZE*BLOCK_SIZE, 1.0f); // Draw a grid
            EndMode3D();
            
            DrawFPS(10, 10);
            DrawText("WASD to move, Mouse to look", 10, 30, 20, BLACK);
            DrawText("Left click to place block, Right click to break", 10, 50, 20, BLACK);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}

void InitWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE])
{
    // Initialize the world with blocks
    for (int x = 0; x < WORLD_SIZE; x++)
    {
        for (int z = 0; z < WORLD_SIZE; z++)
        {
            // Create a simple terrain
            int height = 3 + (int)(perlin((float)x/10.0f, (float)z/10.0f, 0.5f) * 3);
            
            for (int y = 0; y < WORLD_SIZE; y++)
            {
                if (y < height - 2)
                {
                    world[x][y][z].type = BLOCK_STONE;
                    world[x][y][z].active = true;
                }
                else if (y < height)
                {
                    world[x][y][z].type = BLOCK_DIRT;
                    world[x][y][z].active = true;
                }
                else if (y == height)
                {
                    world[x][y][z].type = BLOCK_GRASS;
                    world[x][y][z].active = true;
                }
                else if (y < WORLD_SIZE/2)
                {
                    world[x][y][z].type = BLOCK_WATER;
                    world[x][y][z].active = true;
                }
                else
                {
                    world[x][y][z].type = BLOCK_AIR;
                    world[x][y][z].active = false;
                }
            }
        }
    }
}

void UpdateWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE], Camera3D camera)
{
    // Simple block placement/removal
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        // Calculate the direction the camera is facing
        Vector3 direction = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        
        // Check along the camera direction for block intersection
        for (float t = 0; t < 10.0f; t += 0.1f)
        {
            Vector3 checkPos = Vector3Add(camera.position, Vector3Scale(direction, t));
            
            // Convert to block coordinates
            int x = (int)(checkPos.x / BLOCK_SIZE);
            int y = (int)(checkPos.y / BLOCK_SIZE);
            int z = (int)(checkPos.z / BLOCK_SIZE);
            
            // Check if coordinates are within world bounds
            if (x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE && z >= 0 && z < WORLD_SIZE)
            {
                if (world[x][y][z].active)
                {
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                    {
                        // Break block
                        world[x][y][z].active = false;
                        world[x][y][z].type = BLOCK_AIR;
                        break;
                    }
                }
                else
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        // Place block (one block back along the ray)
                        Vector3 placePos = Vector3Add(camera.position, Vector3Scale(direction, t - 0.1f));
                        int px = (int)(placePos.x / BLOCK_SIZE);
                        int py = (int)(placePos.y / BLOCK_SIZE);
                        int pz = (int)(placePos.z / BLOCK_SIZE);
                        
                        if (px >= 0 && px < WORLD_SIZE && py >= 0 && py < WORLD_SIZE && pz >= 0 && pz < WORLD_SIZE)
                        {
                            world[px][py][pz].active = true;
                            world[px][py][pz].type = BLOCK_GRASS;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void DrawWorld(Block world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE])
{
    for (int x = 0; x < WORLD_SIZE; x++)
    {
        for (int y = 0; y < WORLD_SIZE; y++)
        {
            for (int z = 0; z < WORLD_SIZE; z++)
            {
                if (world[x][y][z].active)
                {
                    Vector3 position = GetBlockPosition(x, y, z);
                    Color color = GetBlockColor(world[x][y][z].type);
                    
                    // Draw the block
                    if (world[x][y][z].type == BLOCK_WATER)
                    {
                        DrawCubeWiresV(position, (Vector3){BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE}, BLUE);
                    }
                    else
                    {
                        DrawCubeV(position, (Vector3){BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE}, color);
                        DrawCubeWiresV(position, (Vector3){BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE}, BLACK);
                    }
                }
            }
        }
    }
}

Color GetBlockColor(BlockType type)
{
    switch (type)
    {
        case BLOCK_GRASS: return GREEN;
        case BLOCK_DIRT: return BROWN;
        case BLOCK_STONE: return GRAY;
        case BLOCK_WATER: return BLUE;
        default: return BLANK;
    }
}

Vector3 GetBlockPosition(int x, int y, int z)
{
    return (Vector3){
        x * BLOCK_SIZE - (WORLD_SIZE * BLOCK_SIZE / 2) + BLOCK_SIZE/2,
        y * BLOCK_SIZE,
        z * BLOCK_SIZE - (WORLD_SIZE * BLOCK_SIZE / 2) + BLOCK_SIZE/2
    };
}

// Simple 2D Perlin noise function for terrain generation
float perlin(float x, float y, float freq)
{
    // This is a very simplified version - for a real game, use a proper Perlin noise implementation
    return 0.5f * sin(x * freq) + 0.5f * cos(y * freq);
}