#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "player.h"

#define MAX_BONES 20
#define MAX_TREES 20

typedef struct Bone {
    Vector3 position;
    bool active;
} Bone;

typedef struct Tree {
    Vector3 position;
} Tree;

void InitWorld(Bone* bones, Tree* trees);
void UpdateWorld(Bone* bones, Tree* trees, Vector3 playerPos);
void DrawWorld3D(Bone* bones, Tree* trees);

#endif