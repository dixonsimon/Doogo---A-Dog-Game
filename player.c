#include "player.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>

void InitDog(Dog* dog) {
    dog->position = (Vector3){ 0.0f, 0.5f, 0.0f }; // Start slightly above ground
    dog->speed = 8.0f;
    dog->verticalSpeed = 0.0f;
    dog->canJump = false;
    dog->rotation = 0.0f;
    dog->score = 0;
    dog->color = BROWN;
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

    // Normalize and Apply
    if (Vector3Length(direction) > 0.1f) {
        direction = Vector3Normalize(direction);
        dog->position = Vector3Add(dog->position, Vector3Scale(direction, dog->speed * deltaTime));
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

    // Draw Body (Local coordinates relative to dog center)
    DrawCube(Vector3Zero(), 1.0f, 1.0f, 2.0f, dog.color);
    DrawCubeWires(Vector3Zero(), 1.0f, 1.0f, 2.0f, DARKBROWN);

    // Draw Head (Offset from center)
    DrawCube((Vector3){ 0.0f, 0.8f, -0.8f }, 0.8f, 0.8f, 0.8f, dog.color);

    // Draw Ears
    Vector3 earL = { -0.3f, 1.3f, -0.8f };
    Vector3 earR = { 0.3f, 1.3f, -0.8f };
    DrawCube(earL, 0.2f, 0.4f, 0.2f, BLACK);
    DrawCube(earR, 0.2f, 0.4f, 0.2f, BLACK);

    // Draw Tail
    Vector3 tailPos = { 0.0f, 0.5f, 1.0f };
    DrawCube(tailPos, 0.2f, 0.2f, 0.5f, BLACK);

    rlPopMatrix();
}