#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "player.h"

#define MAX_BONES 20
#define MAX_TREES 20
#define MAX_MEATS 5

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

void InitWorld(Bone* bones, Tree* trees, Meat* meats);
void UpdateWorld(Bone* bones, Tree* trees, Meat* meats, Vector3 playerPos, int* score, float* health, float maxHealth);
void DrawWorld3D(Bone* bones, Tree* trees, Meat* meats);

#endif