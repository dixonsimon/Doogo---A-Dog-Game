#include "screens.h"
#include "ui.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>

void UpdateTitleScreen(GameState* state, World* world, Dog* dog) {
    state->framesCounter++;
    
    // Rotate camera slowly for the menu background
    state->cameraAngleX += 0.002f;
    state->camera.target = (Vector3){ 0.0f, 0.5f, 0.0f };
    state->camera.position.x = sinf(state->cameraAngleX) * state->cameraDist;
    state->camera.position.z = cosf(state->cameraAngleX) * state->cameraDist;
    state->camera.position.y = 4.0f;

    // --- Menu Logic ---
    int sw = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    // Define Buttons
    Rectangle btnStart = { sw/2 - 100, 250, 200, 40 };
    Rectangle btnSettings = { sw/2 - 100, 300, 200, 40 };
    Rectangle btnQuit = { sw/2 - 100, 350, 200, 40 };

    // Mouse Hover Updates Selection
    if (CheckCollisionPointRec(mousePos, btnStart)) state->menuSelection = 0;
    else if (CheckCollisionPointRec(mousePos, btnSettings)) state->menuSelection = 1;
    else if (CheckCollisionPointRec(mousePos, btnQuit)) state->menuSelection = 2;

    // Keyboard Navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) state->menuSelection = (state->menuSelection + 1) % 3;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) state->menuSelection = (state->menuSelection + 2) % 3;

    // Interaction (Enter or Click)
    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state->menuSelection == 0) {
            InitDog(dog);
            InitWorld(world);
            state->cameraAngleX = 0.0f;
            state->cameraAngleY = 0.4f;
            DisableCursor();
            state->currentScreen = SCREEN_GAMEPLAY;
        } else if (state->menuSelection == 1) {
            state->previousScreen = SCREEN_TITLE;
            state->currentScreen = SCREEN_SETTINGS;
            state->menuSelection = 0;
        } else if (state->menuSelection == 2) {
            state->shouldQuit = true;
        }
    }
}

void DrawTitleScreen(GameState* state, World* world, Dog* dog) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Draw 3D Background
    DrawRectangle(0, 0, sw, sh, SKYBLUE); // Sky
    BeginMode3D(state->camera);
        DrawWorld3D(world);
        DrawDog3D(*dog);
    EndMode3D();

    // Draw UI Overlay
    DrawRectangle(0, 0, sw, sh, (Color){0, 0, 0, 40}); // Slight tint

    // Draw the Game Title
    const char* titleText = "DOOGO";
    int titleWidth = MeasureText(titleText, 80);
    
    // Shadow
    DrawText(titleText, (sw - titleWidth) / 2 + 4, 84, 80, BLACK);
    // Main Text
    DrawText(titleText, (sw - titleWidth) / 2, 80, 80, GOLD);
    
    const char* subTitle = "The Good Boy Adventure";
    int subWidth = MeasureText(subTitle, 30);
    DrawText(subTitle, (sw - subWidth) / 2, 170, 30, RAYWHITE);

    // Draw Buttons
    Rectangle btnStart = { sw/2 - 100, 250, 200, 40 };
    Rectangle btnSettings = { sw/2 - 100, 300, 200, 40 };
    Rectangle btnQuit = { sw/2 - 100, 350, 200, 40 };

    DrawButton(btnStart, "START GAME", state->menuSelection == 0);
    DrawButton(btnSettings, "SETTINGS", state->menuSelection == 1);
    DrawButton(btnQuit, "QUIT", state->menuSelection == 2);
}

void UpdateGameplayScreen(GameState* state, World* world, Dog* dog, float deltaTime, Sound barkSound) {
    // Pause Game
    if (IsKeyPressed(KEY_ESCAPE)) {
        EnableCursor();
        state->currentScreen = SCREEN_PAUSE;
        state->menuSelection = 0; // Reset selection for pause menu
    }

    // --- Camera Input Logic ---
    Vector2 mouseDelta = GetMouseDelta();
    state->cameraAngleX -= mouseDelta.x * 0.005f; // Rotate around Y axis (Yaw)
    state->cameraAngleY += mouseDelta.y * 0.005f; // Rotate Up/Down (Pitch)

    // Clamp Pitch (Don't let camera go under ground or flip over)
    if (state->cameraAngleY > 1.5f) state->cameraAngleY = 1.5f;
    if (state->cameraAngleY < 0.1f) state->cameraAngleY = 0.1f;

    // Calculate Camera Position (Orbit around Dog)
    state->camera.target = (Vector3){ dog->position.x, dog->position.y + 1.0f, dog->position.z };
    
    state->camera.position.x = state->camera.target.x + sinf(state->cameraAngleX) * state->cameraDist * cosf(state->cameraAngleY);
    state->camera.position.z = state->camera.target.z + cosf(state->cameraAngleX) * state->cameraDist * cosf(state->cameraAngleY);
    state->camera.position.y = state->camera.target.y + sinf(state->cameraAngleY) * state->cameraDist;
    // --------------------------

    UpdateDog(dog, deltaTime, state->cameraAngleX);
    
    // Update World and check collisions
    int previousScore = dog->score;
    UpdateWorld(world, &dog->position, &dog->score, &dog->health, dog->maxHealth);

    if (dog->score / 5 > previousScore / 5) {
        PlaySound(barkSound);
    }

    // Check for Death
    if (dog->health <= 0) {
        state->currentScreen = SCREEN_ENDING;
        state->menuSelection = 0;
        EnableCursor();
    }
}

void DrawGameplayScreen(GameState* state, World* world, Dog* dog) {
    BeginMode3D(state->camera);
        DrawWorld3D(world);
        DrawDog3D(*dog);
    EndMode3D();

    // Draw Score
    char scoreText[20];
    sprintf(scoreText, "Bones: %d", dog->score);
    DrawText(scoreText, 20, 20, 20, BLACK);

    // Draw Health Bar
    DrawHealthBar(dog->health, dog->maxHealth, 20, 50, 200, 20);
    // Draw Stamina Bar
    DrawStaminaBar(dog->stamina, dog->maxStamina, 20, 80, 200, 20);
}

void UpdatePauseScreen(GameState* state) {
    int sw = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    Rectangle btnResume = { sw/2 - 100, 180, 200, 40 };
    Rectangle btnSettings = { sw/2 - 100, 230, 200, 40 };
    Rectangle btnQuit = { sw/2 - 100, 280, 200, 40 };

    if (CheckCollisionPointRec(mousePos, btnResume)) state->menuSelection = 0;
    else if (CheckCollisionPointRec(mousePos, btnSettings)) state->menuSelection = 1;
    else if (CheckCollisionPointRec(mousePos, btnQuit)) state->menuSelection = 2;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) state->menuSelection = (state->menuSelection + 1) % 3;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) state->menuSelection = (state->menuSelection + 2) % 3;

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || (state->menuSelection == 0 && IsKeyPressed(KEY_ESCAPE))) {
        if (state->menuSelection == 0) { // Resume
            DisableCursor();
            state->currentScreen = SCREEN_GAMEPLAY;
        } else if (state->menuSelection == 1) { // Settings
            state->previousScreen = SCREEN_PAUSE;
            state->currentScreen = SCREEN_SETTINGS;
            state->menuSelection = 0;
        } else if (state->menuSelection == 2) { // Quit
            state->currentScreen = SCREEN_TITLE;
        }
    }
}

void DrawPauseScreen(GameState* state, World* world, Dog* dog) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Draw the game behind the menu (frozen)
    BeginMode3D(state->camera);
        DrawWorld3D(world);
        DrawDog3D(*dog);
    EndMode3D();

    // Semi-transparent black overlay
    DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 0, 150 });

    // Pause Menu Text
    const char* pauseText = "PAUSED";
    int pauseWidth = MeasureText(pauseText, 40);
    DrawText(pauseText, (sw - pauseWidth) / 2, 100, 40, WHITE);

    Rectangle btnResume = { sw/2 - 100, 180, 200, 40 };
    Rectangle btnSettings = { sw/2 - 100, 230, 200, 40 };
    Rectangle btnQuit = { sw/2 - 100, 280, 200, 40 };

    DrawButton(btnResume, "RESUME", state->menuSelection == 0);
    DrawButton(btnSettings, "SETTINGS", state->menuSelection == 1);
    DrawButton(btnQuit, "QUIT TO TITLE", state->menuSelection == 2);
}

void UpdateSettingsScreen(GameState* state) {
    int sw = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    Rectangle btnFull = { sw/2 - 120, 200, 240, 40 };
    Rectangle btnBack = { sw/2 - 120, 260, 240, 40 };

    if (CheckCollisionPointRec(mousePos, btnFull)) state->menuSelection = 0;
    else if (CheckCollisionPointRec(mousePos, btnBack)) state->menuSelection = 1;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) state->menuSelection = (state->menuSelection + 1) % 2;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) state->menuSelection = (state->menuSelection + 1) % 2;

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state->menuSelection == 0) {
            ToggleFullscreen();
        } else if (state->menuSelection == 1) {
            state->currentScreen = state->previousScreen;
            state->menuSelection = 0;
        }
    }
    // Allow ESC to go back as well
    if (IsKeyPressed(KEY_ESCAPE)) {
        state->currentScreen = state->previousScreen;
        state->menuSelection = 0;
    }
}

void DrawSettingsScreen(GameState* state) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, (Color){ 20, 20, 20, 255 }); // Dark background
    DrawText("SETTINGS", (sw - MeasureText("SETTINGS", 40))/2, 100, 40, WHITE);

    Rectangle btnFull = { sw/2 - 120, 200, 240, 40 };
    Rectangle btnBack = { sw/2 - 120, 260, 240, 40 };

    DrawButton(btnFull, "TOGGLE FULLSCREEN", state->menuSelection == 0);
    DrawButton(btnBack, "BACK", state->menuSelection == 1);
}

void UpdateEndingScreen(GameState* state, World* world, Dog* dog) {
    int sw = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    Rectangle btnRespawn = { sw/2 - 100, 250, 200, 40 };
    Rectangle btnMenu = { sw/2 - 100, 300, 200, 40 };

    if (CheckCollisionPointRec(mousePos, btnRespawn)) state->menuSelection = 0;
    else if (CheckCollisionPointRec(mousePos, btnMenu)) state->menuSelection = 1;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) state->menuSelection = (state->menuSelection + 1) % 2;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) state->menuSelection = (state->menuSelection + 1) % 2;

    if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state->menuSelection == 0) { // Respawn
            InitDog(dog);
            InitWorld(world);
            state->cameraAngleX = 0.0f;
            state->cameraAngleY = 0.4f;
            DisableCursor();
            state->currentScreen = SCREEN_GAMEPLAY;
        } else if (state->menuSelection == 1) { // Main Menu
            state->currentScreen = SCREEN_TITLE;
            state->menuSelection = 0;
        }
    }
}

void DrawEndingScreen(GameState* state) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 0, 200 }); // Dark overlay
    
    const char* text = "YOU DIED";
    int textW = MeasureText(text, 60);
    DrawText(text, (sw - textW)/2, 100, 60, RED);

    Rectangle btnRespawn = { sw/2 - 100, 250, 200, 40 };
    Rectangle btnMenu = { sw/2 - 100, 300, 200, 40 };

    DrawButton(btnRespawn, "RESPAWN", state->menuSelection == 0);
    DrawButton(btnMenu, "MAIN MENU", state->menuSelection == 1);
}
