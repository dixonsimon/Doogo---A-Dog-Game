#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "player.h"

#define MAX_BONES 20
#define MAX_TREES 20
#define MAX_MEATS 5
#define MAX_CLOUDS 30
#define MAX_GRASS 1000

typedef struct Bone {
    Vector3 position;
    bool active;
} Bone;

typedef struct Meat {
    Vector3 position;
    bool active;
} Meat;

typedef struct Tree {
    Vector3 position;
} Tree;

typedef struct Cloud {
    Vector3 position;
    float size;
} Cloud;

typedef struct Grass {
    Vector3 position;
    float size;
} Grass;

typedef struct World {
    Bone bones[MAX_BONES];
    Tree trees[MAX_TREES];
    Meat meats[MAX_MEATS];
    Cloud clouds[MAX_CLOUDS];
    Grass grass[MAX_GRASS];
} World;

void InitWorld(World* world);
void UpdateWorld(World* world, Vector3* playerPos, int* score, float* health, float maxHealth);
void DrawWorld3D(World* world);

#endif