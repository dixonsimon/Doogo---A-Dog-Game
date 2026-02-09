#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct Dog {
    Vector3 position;
    float speed;
    float verticalSpeed;
    bool canJump;
    float rotation;
    int score;
    Color color;
    float stamina;
    float maxStamina;
    float health;
    float maxHealth;
} Dog;

void InitDog(Dog* dog);
void UpdateDog(Dog* dog, float deltaTime, float cameraAngle);
void DrawDog3D(Dog dog);

#endif