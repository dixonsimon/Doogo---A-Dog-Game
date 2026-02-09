#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "player.h"
#include "world.h"

typedef enum GameScreen {
    SCREEN_TITLE = 0,
    SCREEN_GAMEPLAY,
    SCREEN_PAUSE,
    SCREEN_SETTINGS,
    SCREEN_ENDING
} GameScreen;

typedef struct GameState {
    GameScreen currentScreen;
    GameScreen previousScreen;
    int menuSelection;
    Camera3D camera;
    float cameraAngleX;
    float cameraAngleY;
    float cameraDist;
    int framesCounter;
    bool shouldQuit;
} GameState;

void UpdateTitleScreen(GameState* state, World* world, Dog* dog);
void DrawTitleScreen(GameState* state, World* world, Dog* dog);

void UpdateGameplayScreen(GameState* state, World* world, Dog* dog, float deltaTime, Sound barkSound);
void DrawGameplayScreen(GameState* state, World* world, Dog* dog);

void UpdatePauseScreen(GameState* state);
void DrawPauseScreen(GameState* state, World* world, Dog* dog);

void UpdateSettingsScreen(GameState* state);
void DrawSettingsScreen(GameState* state);

void UpdateEndingScreen(GameState* state, World* world, Dog* dog);
void DrawEndingScreen(GameState* state);

#endif
