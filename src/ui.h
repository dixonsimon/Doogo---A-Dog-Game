#ifndef UI_H
#define UI_H

#include "raylib.h"

// Draws a standard button. Returns true if it was clicked (logic handled externally usually, but this helper draws it)
void DrawButton(Rectangle bounds, const char* text, bool selected);

// Draws the health bar
void DrawHealthBar(float health, float maxHealth, int x, int y, int width, int height);

// Draws the stamina bar
void DrawStaminaBar(float stamina, float maxStamina, int x, int y, int width, int height);

#endif