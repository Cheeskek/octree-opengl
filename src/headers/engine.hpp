#ifndef _OCTREE_ENGINE
#define _OCTREE_ENGINE

#include <vector>

struct Node;

void initTree(void (*drawCubeFunction) (int, int, int, int), int depth);
void drawTree(int cur=0, int x=0, int y=0, int z=0, int depth=0);
void update(int x, int y, int z, int scale, bool add, int cur=0, int _x=0, int _y=0, int _z=0, int _depth=0);

bool treeFileOutput(const char* fileName);
bool treeFileInput(const char* filePath);

#endif
