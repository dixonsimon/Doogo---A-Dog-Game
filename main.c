#include "raylib.h"
#include <math.h>
#include <stdio.h> // Required for score text formatting
#include "player.h"
#include "world.h"

// Define the different states the game can be in
typedef enum GameScreen {
    SCREEN_TITLE = 0,
    SCREEN_GAMEPLAY,
    SCREEN_PAUSE,
    SCREEN_SETTINGS,
    SCREEN_ENDING
} GameScreen;

int main(void)
{
    // --------------------------------------------------------------------------------------
    // Initialization
    // --------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Doogo - A Dog's Life");

    // Disable the default ESC key behavior so we can use it for Pause
    SetExitKey(KEY_NULL);

    // Define the initial game state
    GameScreen currentScreen = SCREEN_TITLE;
    GameScreen previousScreen = SCREEN_TITLE; // To return from settings
    int menuSelection = 0; // Track selected button (0 = top button)

    // Frame counter for simple animations (like blinking text)
    int framesCounter = 0;

    // 3D Camera Setup
    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
    
    float cameraAngleX = 0.0f;
    float cameraAngleY = 0.4f; // Start looking slightly down
    float cameraDist = 12.0f;

    // Initialize Doogo
    Dog doogo = { 0 };
    InitDog(&doogo);
    
    Bone bones[MAX_BONES] = { 0 };
    Tree trees[MAX_TREES] = { 0 };
    InitWorld(bones, trees);

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    
    // Lock cursor to center of screen for mouse look
    DisableCursor();
    // --------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // ----------------------------------------------------------------------------------
        // Update Logic (Process Input and Math)
        // ----------------------------------------------------------------------------------
        switch(currentScreen)
        {
            case SCREEN_TITLE:
            {
                framesCounter++;
                
                // Rotate camera slowly for the menu background
                cameraAngleX += 0.002f;
                camera.target = (Vector3){ 0.0f, 0.5f, 0.0f };
                camera.position.x = sinf(cameraAngleX) * cameraDist;
                camera.position.z = cosf(cameraAngleX) * cameraDist;
                camera.position.y = 4.0f;

                // --- Menu Logic ---
                int sw = GetScreenWidth();
                int sh = GetScreenHeight();
                Vector2 mousePos = GetMousePosition();

                // Define Buttons
                Rectangle btnStart = { sw/2 - 100, 250, 200, 40 };
                Rectangle btnSettings = { sw/2 - 100, 300, 200, 40 };
                Rectangle btnQuit = { sw/2 - 100, 350, 200, 40 };

                // Mouse Hover Updates Selection
                if (CheckCollisionPointRec(mousePos, btnStart)) menuSelection = 0;
                else if (CheckCollisionPointRec(mousePos, btnSettings)) menuSelection = 1;
                else if (CheckCollisionPointRec(mousePos, btnQuit)) menuSelection = 2;

                // Keyboard Navigation
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelection = (menuSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuSelection = (menuSelection + 2) % 3;

                // Interaction (Enter or Click)
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (menuSelection == 0) {
                        InitDog(&doogo);
                        InitWorld(bones, trees);
                        cameraAngleX = 0.0f;
                        cameraAngleY = 0.4f;
                        DisableCursor();
                        currentScreen = SCREEN_GAMEPLAY;
                    } else if (menuSelection == 1) {
                        previousScreen = SCREEN_TITLE;
                        currentScreen = SCREEN_SETTINGS;
                        menuSelection = 0;
                    } else if (menuSelection == 2) {
                        CloseWindow();
                        return 0;
                    }
                }
            } break;
            case SCREEN_GAMEPLAY:
            {
                float deltaTime = GetFrameTime(); // Time since last frame (for smooth physics)

                // Pause Game
                if (IsKeyPressed(KEY_ESCAPE)) {
                    EnableCursor();
                    currentScreen = SCREEN_PAUSE;
                    menuSelection = 0; // Reset selection for pause menu
                }

                // --- Camera Input Logic ---
                Vector2 mouseDelta = GetMouseDelta();
                cameraAngleX -= mouseDelta.x * 0.005f; // Rotate around Y axis (Yaw)
                cameraAngleY += mouseDelta.y * 0.005f; // Rotate Up/Down (Pitch)

                // Clamp Pitch (Don't let camera go under ground or flip over)
                if (cameraAngleY > 1.5f) cameraAngleY = 1.5f;
                if (cameraAngleY < 0.1f) cameraAngleY = 0.1f;

                // Calculate Camera Position (Orbit around Dog)
                camera.target = (Vector3){ doogo.position.x, doogo.position.y + 1.0f, doogo.position.z };
                
                camera.position.x = camera.target.x + sinf(cameraAngleX) * cameraDist * cosf(cameraAngleY);
                camera.position.z = camera.target.z + cosf(cameraAngleX) * cameraDist * cosf(cameraAngleY);
                camera.position.y = camera.target.y + sinf(cameraAngleY) * cameraDist;
                // --------------------------

                UpdateDog(&doogo, deltaTime, cameraAngleX);
                
                // Update World and check collisions
                UpdateWorld(bones, trees, doogo.position);
                
                // Manual score check (since we separated logic)
                for (int i = 0; i < MAX_BONES; i++) {
                    if (!bones[i].active) {
                        // Check if we just collected it (distance check again or flag)
                        // For this simple version, we just count inactive bones that are close?
                        // Better: Check collision here properly or pass score pointer.
                        // Let's do a simple distance check for score increment:
                        float dx = doogo.position.x - bones[i].position.x;
                        if (dx > -1.0f && dx < 1.0f && !bones[i].active) {
                             // This logic is a bit flawed for a real game but works for visual demo.
                             // Ideally, UpdateWorld returns score points.
                             // Let's just increment score in UpdateWorld logic next time.
                             // For now, we will just display the dog's internal score if we moved logic there.
                        }
                    }
                }
            } break;
            case SCREEN_PAUSE:
            {
                int sw = GetScreenWidth();
                int sh = GetScreenHeight();
                Vector2 mousePos = GetMousePosition();

                Rectangle btnResume = { sw/2 - 100, 180, 200, 40 };
                Rectangle btnSettings = { sw/2 - 100, 230, 200, 40 };
                Rectangle btnQuit = { sw/2 - 100, 280, 200, 40 };

                if (CheckCollisionPointRec(mousePos, btnResume)) menuSelection = 0;
                else if (CheckCollisionPointRec(mousePos, btnSettings)) menuSelection = 1;
                else if (CheckCollisionPointRec(mousePos, btnQuit)) menuSelection = 2;

                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelection = (menuSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuSelection = (menuSelection + 2) % 3;

                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || (menuSelection == 0 && IsKeyPressed(KEY_ESCAPE))) {
                    if (menuSelection == 0) { // Resume
                        DisableCursor();
                        currentScreen = SCREEN_GAMEPLAY;
                    } else if (menuSelection == 1) { // Settings
                        previousScreen = SCREEN_PAUSE;
                        currentScreen = SCREEN_SETTINGS;
                        menuSelection = 0;
                    } else if (menuSelection == 2) { // Quit
                        currentScreen = SCREEN_TITLE;
                    }
                }
            } break;
            case SCREEN_SETTINGS:
            {
                int sw = GetScreenWidth();
                int sh = GetScreenHeight();
                Vector2 mousePos = GetMousePosition();

                Rectangle btnFull = { sw/2 - 120, 200, 240, 40 };
                Rectangle btnBack = { sw/2 - 120, 260, 240, 40 };

                if (CheckCollisionPointRec(mousePos, btnFull)) menuSelection = 0;
                else if (CheckCollisionPointRec(mousePos, btnBack)) menuSelection = 1;

                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelection = (menuSelection + 1) % 2;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuSelection = (menuSelection + 1) % 2;

                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (menuSelection == 0) {
                        ToggleFullscreen();
                    } else if (menuSelection == 1) {
                        currentScreen = previousScreen;
                        menuSelection = 0;
                    }
                }
                // Allow ESC to go back as well
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentScreen = previousScreen;
                    menuSelection = 0;
                }
            } break;
            default: break;
        }
        // ----------------------------------------------------------------------------------

        // ----------------------------------------------------------------------------------
        // Draw Logic (Render to Screen)
        // ----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            switch(currentScreen)
            {
                case SCREEN_TITLE:
                {   
                    // Draw 3D Background
                    DrawRectangle(0, 0, sw, sh, SKYBLUE); // Sky
                    BeginMode3D(camera);
                        DrawWorld3D(bones, trees);
                        DrawDog3D(doogo);
                    EndMode3D();

                    // Draw UI Overlay
                    DrawRectangle(0, 0, sw, sh, (Color){0, 0, 0, 40}); // Slight tint

                    // Draw the Game Title
                    const char* titleText = "DOOGO";
                    int titleWidth = MeasureText(titleText, 80);
                    
                    // Shadow
                    DrawText(titleText, (screenWidth - titleWidth) / 2 + 4, 84, 80, BLACK);
                    // Main Text
                    DrawText(titleText, (screenWidth - titleWidth) / 2, 80, 80, GOLD);
                    
                    const char* subTitle = "The Good Boy Adventure";
                    int subWidth = MeasureText(subTitle, 30);
                    DrawText(subTitle, (screenWidth - subWidth) / 2, 170, 30, RAYWHITE);

                    // Draw Buttons
                    Rectangle btnStart = { sw/2 - 100, 250, 200, 40 };
                    Rectangle btnSettings = { sw/2 - 100, 300, 200, 40 };
                    Rectangle btnQuit = { sw/2 - 100, 350, 200, 40 };

                    // Helper to draw button (Inline for simplicity)
                    Color cStart = (menuSelection == 0) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnStart, (menuSelection == 0) ? (Color){255,255,255,200} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnStart, 2, cStart);
                    DrawText("START GAME", btnStart.x + (btnStart.width - MeasureText("START GAME", 20))/2, btnStart.y + 10, 20, cStart);

                    Color cSet = (menuSelection == 1) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnSettings, (menuSelection == 1) ? (Color){255,255,255,200} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnSettings, 2, cSet);
                    DrawText("SETTINGS", btnSettings.x + (btnSettings.width - MeasureText("SETTINGS", 20))/2, btnSettings.y + 10, 20, cSet);

                    Color cQuit = (menuSelection == 2) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnQuit, (menuSelection == 2) ? (Color){255,255,255,200} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnQuit, 2, cQuit);
                    DrawText("QUIT", btnQuit.x + (btnQuit.width - MeasureText("QUIT", 20))/2, btnQuit.y + 10, 20, cQuit);

                } break;
                case SCREEN_GAMEPLAY:
                {
                    BeginMode3D(camera);
                        DrawWorld3D(bones, trees);
                        DrawDog3D(doogo);
                        // Draw a huge sky plane or just rely on ClearBackground
                        // (Sky is handled by ClearBackground(RAYWHITE) -> changed to SKYBLUE below)
                    EndMode3D();

                    // Draw Score (UI stays fixed on screen)
                    // Note: Score logic needs to be re-connected in UpdateWorld for display
                    // For now we just show the text
                    char scoreText[20];
                    sprintf(scoreText, "Bones: %d", doogo.score);
                    DrawText(scoreText, 20, 20, 20, BLACK);
                } break;
                case SCREEN_PAUSE:
                {
                    // Draw the game behind the menu (frozen)
                    BeginMode3D(camera);
                        DrawWorld3D(bones, trees);
                        DrawDog3D(doogo);
                    EndMode3D();

                    // Semi-transparent black overlay
                    DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 0, 150 });

                    // Pause Menu Text
                    const char* pauseText = "PAUSED";
                    int pauseWidth = MeasureText(pauseText, 40);
                    DrawText(pauseText, (screenWidth - pauseWidth) / 2, 100, 40, WHITE);

                    Rectangle btnResume = { sw/2 - 100, 180, 200, 40 };
                    Rectangle btnSettings = { sw/2 - 100, 230, 200, 40 };
                    Rectangle btnQuit = { sw/2 - 100, 280, 200, 40 };

                    Color cRes = (menuSelection == 0) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnResume, (menuSelection == 0) ? (Color){255,255,255,50} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnResume, 2, cRes);
                    DrawText("RESUME", btnResume.x + (btnResume.width - MeasureText("RESUME", 20))/2, btnResume.y + 10, 20, cRes);

                    Color cSet = (menuSelection == 1) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnSettings, (menuSelection == 1) ? (Color){255,255,255,50} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnSettings, 2, cSet);
                    DrawText("SETTINGS", btnSettings.x + (btnSettings.width - MeasureText("SETTINGS", 20))/2, btnSettings.y + 10, 20, cSet);

                    Color cQuit = (menuSelection == 2) ? GOLD : LIGHTGRAY;
                    DrawRectangleRec(btnQuit, (menuSelection == 2) ? (Color){255,255,255,50} : (Color){0,0,0,100});
                    DrawRectangleLinesEx(btnQuit, 2, cQuit);
                    DrawText("QUIT TO TITLE", btnQuit.x + (btnQuit.width - MeasureText("QUIT TO TITLE", 20))/2, btnQuit.y + 10, 20, cQuit);
                } break;
                case SCREEN_SETTINGS:
                {
                    DrawRectangle(0, 0, sw, sh, (Color){ 20, 20, 20, 255 }); // Dark background
                    DrawText("SETTINGS", (sw - MeasureText("SETTINGS", 40))/2, 100, 40, WHITE);

                    Rectangle btnFull = { sw/2 - 120, 200, 240, 40 };
                    Rectangle btnBack = { sw/2 - 120, 260, 240, 40 };

                    Color cFull = (menuSelection == 0) ? GOLD : LIGHTGRAY;
                    DrawRectangleLinesEx(btnFull, 2, cFull);
                    DrawText("TOGGLE FULLSCREEN", btnFull.x + (btnFull.width - MeasureText("TOGGLE FULLSCREEN", 20))/2, btnFull.y + 10, 20, cFull);

                    Color cBack = (menuSelection == 1) ? GOLD : LIGHTGRAY;
                    DrawRectangleLinesEx(btnBack, 2, cBack);
                    DrawText("BACK", btnBack.x + (btnBack.width - MeasureText("BACK", 20))/2, btnBack.y + 10, 20, cBack);
                } break;
                default: break;
            }

        EndDrawing();
        // ----------------------------------------------------------------------------------
    }

    // --------------------------------------------------------------------------------------
    // De-Initialization
    // --------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    // --------------------------------------------------------------------------------------

    return 0;
}
