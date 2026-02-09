#include "ui.h"

void DrawButton(Rectangle bounds, const char* text, bool selected) {
    Color color = selected ? GOLD : LIGHTGRAY;
    // Draw background with transparency
    DrawRectangleRec(bounds, selected ? (Color){255, 255, 255, 200} : (Color){0, 0, 0, 100});
    // Draw border
    DrawRectangleLinesEx(bounds, 2, color);
    
    // Center text
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (int)(bounds.x + (bounds.width - textWidth) / 2), (int)(bounds.y + (bounds.height - fontSize) / 2), fontSize, color);
}

void DrawHealthBar(float health, float maxHealth, int x, int y, int width, int height) {
    DrawRectangle(x, y, width, height, GRAY);
    if (maxHealth > 0) {
        DrawRectangle(x, y, (int)(width * (health / maxHealth)), height, RED);
    }
    DrawText("Health", x + 5, y + 2, 10, WHITE);
}

void DrawStaminaBar(float stamina, float maxStamina, int x, int y, int width, int height) {
    DrawRectangle(x, y, width, height, GRAY);
    if (maxStamina > 0) {
        DrawRectangle(x, y, (int)(width * (stamina / maxStamina)), height, GREEN);
    }
    DrawText("Stamina", x + 5, y + 2, 10, WHITE);
}