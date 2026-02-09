#include "player.h"
#include "raymath.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>

void InitDog(Dog* dog) {
    dog->position = (Vector3){ 0.0f, 0.5f, 0.0f }; // Start slightly above ground
    dog->speed = 8.0f;
    dog->verticalSpeed = 0.0f;
    dog->canJump = false;
    dog->rotation = 0.0f;
    dog->score = 0;
    dog->color = BROWN;
    dog->stamina = 100.0f;
    dog->maxStamina = 100.0f;
    dog->health = 100.0f;
    dog->maxHealth = 100.0f;
}

void UpdateDog(Dog* dog, float deltaTime, float cameraAngle) {
    dog->rotation = cameraAngle;

    // Calculate movement vectors based on Camera Angle (Yaw)
    // In this setup, sin/cos logic assumes camera orbits using standard trig
    float sinA = sinf(cameraAngle);
    float cosA = cosf(cameraAngle);

    // Forward vector (Where camera is looking on XZ plane)
    Vector3 forward = { -sinA, 0.0f, -cosA };
    // Right vector (90 degrees to the right)
    Vector3 right = { cosA, 0.0f, -sinA };

    Vector3 direction = { 0 };

    // 3D Movement (Relative to Camera)
    if (IsKeyDown(KEY_W)) direction = Vector3Add(direction, forward);
    if (IsKeyDown(KEY_S)) direction = Vector3Subtract(direction, forward);
    if (IsKeyDown(KEY_D)) direction = Vector3Add(direction, right);
    if (IsKeyDown(KEY_A)) direction = Vector3Subtract(direction, right);

    float currentSpeed = dog->speed;

    // Sprinting Logic
    if (IsKeyDown(KEY_LEFT_SHIFT) && dog->stamina > 0) {
        currentSpeed *= 1.8f; // Sprint multiplier
        dog->stamina -= 30.0f * deltaTime; // Drain stamina
        if (dog->stamina < 0) dog->stamina = 0;
    } else if (dog->stamina < dog->maxStamina && dog->health > 0) {
        // Refill stamina using health
        float regen = 15.0f * deltaTime;
        dog->stamina += regen;
        dog->health -= regen * 0.2f; // Health cost for stamina regen
        if (dog->stamina > dog->maxStamina) dog->stamina = dog->maxStamina;
        if (dog->health < 0) dog->health = 0;
    }

    // Normalize and Apply
    if (Vector3Length(direction) > 0.1f) {
        direction = Vector3Normalize(direction);
        dog->position = Vector3Add(dog->position, Vector3Scale(direction, currentSpeed * deltaTime));
    }

    // Jump
    if (IsKeyPressed(KEY_SPACE) && dog->canJump) {
        dog->verticalSpeed = 12.0f;
        dog->canJump = false;
    }

    // Gravity
    dog->position.y += dog->verticalSpeed * deltaTime;
    dog->verticalSpeed -= 30.0f * deltaTime; // Gravity constant

    // Ground Collision (Ground is at Y = 0)
    if (dog->position.y <= 0.5f) {
        dog->position.y = 0.5f;
        dog->verticalSpeed = 0.0f;
        dog->canJump = true;
    }
}

void DrawDog3D(Dog dog) {
    rlPushMatrix();
    rlTranslatef(dog.position.x, dog.position.y, dog.position.z);
    rlRotatef(dog.rotation * RAD2DEG, 0.0f, 1.0f, 0.0f);

    // High Poly Dog Model Construction
    
    // Body (Capsule-like using Cylinder + Spheres)
    Vector3 bodyStart = {0.0f, 0.0f, -0.6f};
    Vector3 bodyEnd = {0.0f, 0.0f, 0.6f};
    DrawCylinderEx(bodyStart, bodyEnd, 0.5f, 0.5f, 16, dog.color);
    DrawSphereEx(bodyStart, 0.5f, 16, 16, dog.color);
    DrawSphereEx(bodyEnd, 0.5f, 16, 16, dog.color);

    // Head
    Vector3 headPos = {0.0f, 0.8f, -0.8f};
    DrawSphereEx(headPos, 0.45f, 16, 16, dog.color);
    
    // Snout
    Vector3 snoutPos = {0.0f, 0.7f, -1.2f};
    DrawCylinderEx(headPos, snoutPos, 0.4f, 0.2f, 16, dog.color);
    DrawSphereEx(snoutPos, 0.2f, 12, 12, BLACK); // Nose

    // Draw Ears
    Vector3 earLBase = {-0.3f, 1.1f, -0.8f};
    Vector3 earRBase = {0.3f, 1.1f, -0.8f};
    DrawCylinderEx(earLBase, (Vector3){-0.4f, 1.4f, -0.8f}, 0.1f, 0.0f, 8, DARKBROWN);
    DrawCylinderEx(earRBase, (Vector3){0.4f, 1.4f, -0.8f}, 0.1f, 0.0f, 8, DARKBROWN);

    // Legs (4 cylinders)
    float legRadius = 0.15f;
    DrawCylinderEx((Vector3){-0.3f, 0.0f, -0.5f}, (Vector3){-0.3f, -0.5f, -0.5f}, legRadius, 0.1f, 12, dog.color);
    DrawCylinderEx((Vector3){0.3f, 0.0f, -0.5f}, (Vector3){0.3f, -0.5f, -0.5f}, legRadius, 0.1f, 12, dog.color);
    DrawCylinderEx((Vector3){-0.3f, 0.0f, 0.5f}, (Vector3){-0.3f, -0.5f, 0.5f}, legRadius, 0.1f, 12, dog.color);
    DrawCylinderEx((Vector3){0.3f, 0.0f, 0.5f}, (Vector3){0.3f, -0.5f, 0.5f}, legRadius, 0.1f, 12, dog.color);

    // Draw Tail
    Vector3 tailStart = {0.0f, 0.2f, 0.9f};
    Vector3 tailEnd = {0.0f, 0.6f, 1.3f};
    DrawCylinderEx(tailStart, tailEnd, 0.15f, 0.05f, 12, dog.color);

    rlPopMatrix();
}