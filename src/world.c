#include "world.h"
#include "raymath.h"
#include <math.h>

void InitWorld(World* world) {
    // Initialize Trees scattered around
    for (int i = 0; i < MAX_TREES; i++) {
        world->trees[i].position = (Vector3){ 
            (float)GetRandomValue(-50, 50), 
            0.0f, 
            (float)GetRandomValue(-10, 10) 
        };
    }

    // Initialize Bones
    for (int i = 0; i < MAX_BONES; i++) {
        world->bones[i].position = (Vector3){ 
            (float)GetRandomValue(10, 100), 
            0.5f, 
            (float)GetRandomValue(-5, 5) 
        };
        world->bones[i].active = true;
    }

    // Initialize Meats (Rare)
    for (int i = 0; i < MAX_MEATS; i++) {
        world->meats[i].position = (Vector3){ 
            (float)GetRandomValue(10, 100), 
            0.5f, 
            (float)GetRandomValue(-5, 5) 
        };
        world->meats[i].active = true;
    }

    // Initialize Clouds
    for (int i = 0; i < MAX_CLOUDS; i++) {
        world->clouds[i].position = (Vector3){
            (float)GetRandomValue(-100, 100),
            (float)GetRandomValue(15, 25),
            (float)GetRandomValue(-100, 100)
        };
        world->clouds[i].size = (float)GetRandomValue(20, 50) / 10.0f;
    }

    // Initialize Grass
    for (int i = 0; i < MAX_GRASS; i++) {
        world->grass[i].position = (Vector3){
            (float)GetRandomValue(-60, 60),
            0.0f,
            (float)GetRandomValue(-60, 60)
        };
        world->grass[i].size = (float)GetRandomValue(5, 15) / 10.0f;
    }
}

void UpdateWorld(World* world, Vector3* playerPos, int* score, float* health, float maxHealth) {
    // Infinite World Logic (Omnidirectional):
    // If an object gets too far from the player, move it to a new random spot nearby.
    float maxDistance = 60.0f;

    for (int i = 0; i < MAX_TREES; i++) {
        if (Vector3Distance(world->trees[i].position, *playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(40, 55);
            world->trees[i].position.x = playerPos->x + sinf(angle) * dist;
            world->trees[i].position.z = playerPos->z + cosf(angle) * dist;
        }

        // Tree Collision (Solid Object)
        // Simple circle collision response
        float dist = Vector3Distance((Vector3){playerPos->x, 0, playerPos->z}, (Vector3){world->trees[i].position.x, 0, world->trees[i].position.z});
        float collisionDist = 1.5f; // Tree radius (~1.0) + Player radius (~0.5)
        if (dist < collisionDist) {
            Vector3 dir = Vector3Subtract(*playerPos, world->trees[i].position);
            dir.y = 0;
            if (Vector3Length(dir) > 0) {
                dir = Vector3Normalize(dir);
                // Push player out
                *playerPos = Vector3Add(world->trees[i].position, Vector3Scale(dir, collisionDist));
            }
        }
    }

    for (int i = 0; i < MAX_BONES; i++) {
        if (Vector3Distance(world->bones[i].position, *playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(40, 55);
            world->bones[i].position.x = playerPos->x + sinf(angle) * dist;
            world->bones[i].position.z = playerPos->z + cosf(angle) * dist;
            world->bones[i].active = true;
        }

        // Collision Detection (Simple AABB)
        if (world->bones[i].active) {
            // Check distance between dog and bone
            float dx = playerPos->x - world->bones[i].position.x;
            float dz = playerPos->z - world->bones[i].position.z;
            float dy = playerPos->y - world->bones[i].position.y;
            
            // If close enough, collect it
            if (dx*dx + dz*dz + dy*dy < 2.0f) {
                world->bones[i].active = false;
                if (score) (*score)++;
            }
        }
    }

    for (int i = 0; i < MAX_MEATS; i++) {
        if (Vector3Distance(world->meats[i].position, *playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(40, 55);
            world->meats[i].position.x = playerPos->x + sinf(angle) * dist;
            world->meats[i].position.z = playerPos->z + cosf(angle) * dist;
            world->meats[i].active = true;
        }

        // Collision Detection (Simple AABB)
        if (world->meats[i].active) {
            float dx = playerPos->x - world->meats[i].position.x;
            float dz = playerPos->z - world->meats[i].position.z;
            float dy = playerPos->y - world->meats[i].position.y;
            
            if (dx*dx + dz*dz + dy*dy < 2.0f) {
                world->meats[i].active = false;
                if (health) {
                    *health += 30.0f; // Restore health
                    if (*health > maxHealth) *health = maxHealth;
                }
            }
        }
    }

    // Update Grass (Infinite)
    for (int i = 0; i < MAX_GRASS; i++) {
        if (Vector3Distance(world->grass[i].position, *playerPos) > maxDistance) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(40, 55);
            world->grass[i].position.x = playerPos->x + sinf(angle) * dist;
            world->grass[i].position.z = playerPos->z + cosf(angle) * dist;
        }
    }

    // Update Clouds (Drift)
    for (int i = 0; i < MAX_CLOUDS; i++) {
        world->clouds[i].position.x += 0.02f; // Wind
        if (Vector3Distance(world->clouds[i].position, *playerPos) > 120.0f) {
             float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
             float dist = (float)GetRandomValue(80, 100);
             world->clouds[i].position.x = playerPos->x + sinf(angle) * dist;
             world->clouds[i].position.z = playerPos->z + cosf(angle) * dist;
        }
    }
}

void DrawWorld3D(World* world) {
    // Draw Ground Plane
    DrawPlane((Vector3){0, 0, 0}, (Vector2){1000.0f, 1000.0f}, (Color){50, 160, 50, 255}); // Grassy Green

    // Draw Grass (Ambient)
    for (int i = 0; i < MAX_GRASS; i++) {
        Vector3 pos = world->grass[i].position;
        float size = world->grass[i].size;
        
        // Draw a complex clump of grass blades
        int bladeCount = 5;
        for (int j = 0; j < bladeCount; j++) {
            // Deterministic random-like values based on index i and blade j
            float angle = (i * 7 + j * (360 / bladeCount)) * DEG2RAD;
            float lean = 0.2f * size;
            float height = (0.4f + 0.2f * sinf(i * j)) * size;
            
            Vector3 start = { pos.x + sinf(angle)*0.05f, pos.y, pos.z + cosf(angle)*0.05f };
            Vector3 end = { start.x + sinf(angle) * lean, start.y + height, start.z + cosf(angle) * lean };
            
            // Vary color slightly
            Color bladeColor = (j % 2 == 0) ? (Color){30, 120, 30, 255} : (Color){40, 140, 40, 255};
            DrawCylinderEx(start, end, 0.03f * size, 0.0f, 3, bladeColor);
        }
    }

    // Draw Trees
    for (int i = 0; i < MAX_TREES; i++) {
        Vector3 pos = world->trees[i].position;
        
        // Main Trunk
        DrawCylinderEx(pos, (Vector3){pos.x, pos.y + 2.5f, pos.z}, 0.6f, 0.4f, 8, DARKBROWN);
        
        // Branches
        Vector3 branch1Start = {pos.x, pos.y + 1.5f, pos.z};
        Vector3 branch1End = {pos.x + 1.2f, pos.y + 2.5f, pos.z};
        DrawCylinderEx(branch1Start, branch1End, 0.2f, 0.05f, 6, DARKBROWN);

        Vector3 branch2Start = {pos.x, pos.y + 1.8f, pos.z};
        Vector3 branch2End = {pos.x - 1.0f, pos.y + 2.8f, pos.z + 0.5f};
        DrawCylinderEx(branch2Start, branch2End, 0.2f, 0.05f, 6, DARKBROWN);

        // Leaves - Clusters of spheres
        Color leafColor1 = {0, 100, 0, 255};
        Color leafColor2 = {0, 120, 0, 255};
        
        // Top foliage
        DrawSphereEx((Vector3){pos.x, pos.y + 3.5f, pos.z}, 1.5f, 8, 8, leafColor1);
        DrawSphereEx((Vector3){pos.x + 0.8f, pos.y + 3.0f, pos.z}, 1.2f, 8, 8, leafColor2);
        DrawSphereEx((Vector3){pos.x - 0.8f, pos.y + 3.2f, pos.z + 0.5f}, 1.2f, 8, 8, leafColor1);
        DrawSphereEx((Vector3){pos.x, pos.y + 3.2f, pos.z - 0.8f}, 1.2f, 8, 8, leafColor2);
        
        // Branch foliage
        DrawSphereEx(branch1End, 0.8f, 6, 6, leafColor2);
        DrawSphereEx(branch2End, 0.8f, 6, 6, leafColor1);
    }

    // Draw Bones
    for (int i = 0; i < MAX_BONES; i++) {
        if (world->bones[i].active) {
            Vector3 pos = world->bones[i].position;
            // Shaft
            Vector3 start = {pos.x - 0.4f, pos.y, pos.z};
            Vector3 end = {pos.x + 0.4f, pos.y, pos.z};
            DrawCylinderEx(start, end, 0.1f, 0.1f, 8, RAYWHITE);
            
            // Knobs (Left)
            DrawSphereEx((Vector3){start.x, start.y - 0.1f, start.z - 0.1f}, 0.15f, 8, 8, RAYWHITE);
            DrawSphereEx((Vector3){start.x, start.y + 0.1f, start.z + 0.1f}, 0.15f, 8, 8, RAYWHITE);
            
            // Knobs (Right)
            DrawSphereEx((Vector3){end.x, end.y - 0.1f, end.z - 0.1f}, 0.15f, 8, 8, RAYWHITE);
            DrawSphereEx((Vector3){end.x, end.y + 0.1f, end.z + 0.1f}, 0.15f, 8, 8, RAYWHITE);
        }
    }

    // Draw Meats
    for (int i = 0; i < MAX_MEATS; i++) {
        if (world->meats[i].active) {
            Vector3 pos = world->meats[i].position;
            
            // Bone sticking out
            DrawCylinderEx((Vector3){pos.x, pos.y, pos.z}, (Vector3){pos.x + 0.6f, pos.y, pos.z}, 0.08f, 0.08f, 8, RAYWHITE);
            
            // Meat chunk (Cylinder + Spheres to round it)
            Vector3 meatStart = {pos.x - 0.2f, pos.y, pos.z};
            Vector3 meatEnd = {pos.x + 0.3f, pos.y, pos.z};
            DrawCylinderEx(meatStart, meatEnd, 0.25f, 0.3f, 12, MAROON);
            DrawSphereEx(meatStart, 0.25f, 12, 12, RED);
            DrawSphereEx(meatEnd, 0.3f, 12, 12, RED);
            
            // Fat marbling (simple white bands)
            DrawCylinderEx((Vector3){pos.x, pos.y, pos.z}, (Vector3){pos.x + 0.05f, pos.y, pos.z}, 0.31f, 0.31f, 12, WHITE);
        }
    }

    // Draw Clouds
    for (int i = 0; i < MAX_CLOUDS; i++) {
        Vector3 pos = world->clouds[i].position;
        float size = world->clouds[i].size;
        Color cloudColor = (Color){255, 255, 255, 220};
        
        // Central puff
        DrawSphereEx(pos, size, 12, 12, cloudColor);
        
        // Surrounding puffs
        DrawSphereEx((Vector3){pos.x + size*0.8f, pos.y - size*0.2f, pos.z}, size * 0.7f, 10, 10, cloudColor);
        DrawSphereEx((Vector3){pos.x - size*0.8f, pos.y - size*0.1f, pos.z}, size * 0.7f, 10, 10, cloudColor);
        DrawSphereEx((Vector3){pos.x, pos.y + size*0.3f, pos.z + size*0.7f}, size * 0.6f, 10, 10, cloudColor);
        DrawSphereEx((Vector3){pos.x, pos.y + size*0.2f, pos.z - size*0.7f}, size * 0.6f, 10, 10, cloudColor);
    }
}