#include "world.h"
#include "raymath.h"
#include <math.h>

void InitWorld(Bone* bones, Tree* trees, Meat* meats) {
    // Initialize Trees scattered around
    for (int i = 0; i < MAX_TREES; i++) {
        trees[i].position = (Vector3){ 
            (float)GetRandomValue(-50, 50), 
            0.0f, 
            (float)GetRandomValue(-10, 10) 
        };
    }

    // Initialize Bones
    for (int i = 0; i < MAX_BONES; i++) {
        bones[i].position = (Vector3){ 
            (float)GetRandomValue(10, 100), 
            0.5f, 
            (float)GetRandomValue(-5, 5) 
        };
        bones[i].active = true;
    }

    // Initialize Meats (Rare)
    for (int i = 0; i < MAX_MEATS; i++) {
        meats[i].position = (Vector3){ 
            (float)GetRandomValue(10, 100), 
            0.5f, 
            (float)GetRandomValue(-5, 5) 
        };
        meats[i].active = true;
    }
}

void UpdateWorld(Bone* bones, Tree* trees, Meat* meats, Vector3 playerPos, int* score, float* health, float maxHealth) {
    // Infinite World Logic (Omnidirectional):
    // If an object gets too far from the player, move it to a new random spot nearby.
    float maxDistance = 50.0f;

    for (int i = 0; i < MAX_TREES; i++) {
        if (Vector3Distance(trees[i].position, playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(30, 45);
            trees[i].position.x = playerPos.x + sinf(angle) * dist;
            trees[i].position.z = playerPos.z + cosf(angle) * dist;
        }
    }

    for (int i = 0; i < MAX_BONES; i++) {
        if (Vector3Distance(bones[i].position, playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(30, 45);
            bones[i].position.x = playerPos.x + sinf(angle) * dist;
            bones[i].position.z = playerPos.z + cosf(angle) * dist;
            bones[i].active = true;
        }

        // Collision Detection (Simple AABB)
        if (bones[i].active) {
            // Check distance between dog and bone
            float dx = playerPos.x - bones[i].position.x;
            float dz = playerPos.z - bones[i].position.z;
            float dy = playerPos.y - bones[i].position.y;
            
            // If close enough, collect it
            if (dx*dx + dz*dz + dy*dy < 2.0f) {
                bones[i].active = false;
                if (score) (*score)++;
            }
        }
    }

    for (int i = 0; i < MAX_MEATS; i++) {
        if (Vector3Distance(meats[i].position, playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(30, 45);
            meats[i].position.x = playerPos.x + sinf(angle) * dist;
            meats[i].position.z = playerPos.z + cosf(angle) * dist;
            meats[i].active = true;
        }

        // Collision Detection (Simple AABB)
        if (meats[i].active) {
            float dx = playerPos.x - meats[i].position.x;
            float dz = playerPos.z - meats[i].position.z;
            float dy = playerPos.y - meats[i].position.y;
            
            if (dx*dx + dz*dz + dy*dy < 2.0f) {
                meats[i].active = false;
                if (health) {
                    *health += 30.0f; // Restore health
                    if (*health > maxHealth) *health = maxHealth;
                }
            }
        }
    }
}

void DrawWorld3D(Bone* bones, Tree* trees, Meat* meats) {
    // Draw Ground Plane
    DrawPlane((Vector3){0, 0, 0}, (Vector2){1000.0f, 100.0f}, LIME);

    // Draw Trees
    for (int i = 0; i < MAX_TREES; i++) {
        DrawCylinder(trees[i].position, 0.5f, 0.5f, 4.0f, 6, DARKBROWN); // Trunk
        DrawSphere((Vector3){trees[i].position.x, 3.0f, trees[i].position.z}, 2.0f, DARKGREEN); // Leaves
    }

    // Draw Bones
    for (int i = 0; i < MAX_BONES; i++) {
        if (bones[i].active) {
            DrawCube(bones[i].position, 0.5f, 0.2f, 0.2f, RAYWHITE);
        }
    }

    // Draw Meats
    for (int i = 0; i < MAX_MEATS; i++) {
        if (meats[i].active) {
            DrawCube(meats[i].position, 0.4f, 0.4f, 0.4f, RED); // Meat looks like red cube
        }
    }
}