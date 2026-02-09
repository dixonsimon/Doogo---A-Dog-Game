#include "raylib.h"
#include <math.h>
#include <stdio.h> // Required for score text formatting
#include "player.h"
#include "world.h"
#include "screens.h"
#include "ui.h"

int main(void)
{
    // --------------------------------------------------------------------------------------
    // Initialization
    // --------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Doogo - A Dog's Life");
    InitAudioDevice();

    // Disable the default ESC key behavior so we can use it for Pause
    SetExitKey(KEY_NULL);

    // Initialize Game State
    GameState gameState = { 0 };
    gameState.currentScreen = SCREEN_TITLE;
    gameState.previousScreen = SCREEN_TITLE;
    gameState.menuSelection = 0;
    gameState.shouldQuit = false;

    // 3D Camera Setup
    gameState.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    gameState.camera.fovy = 45.0f;
    gameState.camera.projection = CAMERA_PERSPECTIVE;
    gameState.cameraAngleY = 0.4f;
    gameState.cameraDist = 12.0f;

    // Initialize Doogo
    Dog doogo = { 0 };
    InitDog(&doogo);
    
    World world = { 0 };
    InitWorld(&world);

    Sound barkSound = LoadSound("asset/audio/bark.mp3");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    
    // Lock cursor to center of screen for mouse look
    DisableCursor();
    // --------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !gameState.shouldQuit)    // Detect window close button or ESC key
    {
        // ----------------------------------------------------------------------------------
        // Update Logic (Process Input and Math)
        // ----------------------------------------------------------------------------------
        switch(gameState.currentScreen)
        {
            case SCREEN_TITLE:
                UpdateTitleScreen(&gameState, &world, &doogo);
                break;
            case SCREEN_GAMEPLAY:
                UpdateGameplayScreen(&gameState, &world, &doogo, GetFrameTime(), barkSound);
                break;
            case SCREEN_PAUSE:
                UpdatePauseScreen(&gameState);
                break;
            case SCREEN_SETTINGS:
                UpdateSettingsScreen(&gameState);
                break;
            case SCREEN_ENDING:
                UpdateEndingScreen(&gameState, &world, &doogo);
                break;
            default: break;
        }
        // ----------------------------------------------------------------------------------

        // ----------------------------------------------------------------------------------
        // Draw Logic (Render to Screen)
        // ----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch(gameState.currentScreen)
            {
                case SCREEN_TITLE:
                    DrawTitleScreen(&gameState, &world, &doogo);
                    break;
                case SCREEN_GAMEPLAY:
                    DrawGameplayScreen(&gameState, &world, &doogo);
                    break;
                case SCREEN_PAUSE:
                    DrawPauseScreen(&gameState, &world, &doogo);
                    break;
                case SCREEN_SETTINGS:
                    DrawSettingsScreen(&gameState);
                    break;
                case SCREEN_ENDING:
                    DrawEndingScreen(&gameState);
                    break;
                default: break;
            }

        EndDrawing();
        // ----------------------------------------------------------------------------------
    }

    // --------------------------------------------------------------------------------------
    // De-Initialization
    // --------------------------------------------------------------------------------------
    UnloadSound(barkSound);
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------------

    return 0;
}
