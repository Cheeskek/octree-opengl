#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <thread>

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

struct Node {
    bool stop : 1;
    bool draw : 1;
    Node() : stop(false), draw(false) {}
};

namespace {
    int worldSize;
    std::vector<Node> tree;
    int treeDepth;
    void (*drawCube) (int, int, int, int);
}

inline int getScale(int depth) {
    return (worldSize >> depth);
}

void initTree(void (*drawCubeFunction) (int, int, int, int), int depth = 4) {
    treeDepth = depth;
    worldSize = 1 << treeDepth;
    int siz = 0;
    for (int i = 0; i <= depth; ++i) {
        siz += (1 << (i * 3));
    }
    tree.resize(siz);
    drawCube = drawCubeFunction;
    tree[0].stop = true;
}

// opencl сделай

void drawTree(int cur = 0, int x = 0, int y = 0, int z = 0, int depth = 0) {
    if (cur >= tree.size()) {
        return;
    }
    if (tree[cur].draw) {
        drawCube(x, y, z, getScale(depth));
    }
    if (tree[cur].stop) {
        return;
    }
    for (int i = 1; i <= 8; ++i) {
        drawTree((cur << 3) + i, x + ((i >> 0) & 1) * getScale(depth + 1),
                                         y + ((i >> 1) & 1) * getScale(depth + 1),
                                         z + ((i >> 2) & 1) * getScale(depth + 1), depth + 1);
    }
}

void update(int x, int y, int z, int scale, bool add,
        int cur = 0, int _x = 0, int _y = 0, int _z = 0, int _depth = 0) {
    // чтобы не выйти за границы
    if (_depth > treeDepth) {
        return;
    }
    // если не попал
    if (x + scale <= _x || _x + getScale(_depth) <= x ||
            y + scale <= _y || _y + getScale(_depth) <= y ||
            z + scale <= _z || _z + getScale(_depth) <= z) {
        return;
    }
    // если попал полностью
    if (x <= _x && _x + getScale(_depth) <= x + scale &&
            y <= _y && _y + getScale(_depth) <= y + scale &&
            z <= _z && _z + getScale(_depth) <= z + scale) {
        tree[cur].draw = add;
        tree[cur].stop = true;
        return;
    }
    // пуш вниз по дереву, чтобы не удалить лишнего
    if (tree[cur].stop) {
        if (tree[cur].draw != add) {
            for (int i = 1; i <= 8; ++i) {
                tree[(cur << 3) + i].draw = tree[cur].draw;
                tree[(cur << 3) + i].stop = true;
            }
            tree[cur].draw = false;
            tree[cur].stop = false;
        }
        else {
            return;
        }
    }
    // спуск по дереву
    for (int i = 1; i <= 8; ++i) {
        update(x, y, z, scale, add, (cur << 3) + i, _x + ((i >> 0) & 1) * getScale(_depth + 1),
                                                    _y + ((i >> 1) & 1) * getScale(_depth + 1),
                                                    _z + ((i >> 2) & 1) * getScale(_depth + 1), _depth + 1);
    }
    // поднятие вверх, чтобы не сделать больше шагов, чем надо
    bool end = true;
    int ans = 0;
    for (int i = 1; i <= 8; ++i) {
        if (!tree[cur].stop) {
            end = false;
            break;
        }
        ans += tree[cur].draw;
    }
    if (end) {
        if (ans == 8) {
            tree[cur].draw = true;
        }
        else if (ans == 0) {
            tree[cur].draw = false;
        }
        tree[cur].stop = true;
    }
}

// про файлы
//      сначала пишется int с глубиной дерева
//      затем куча char'ов, в каждом сначала draw, затем stop, каждый по биту
//      они упаковываются в char'ы, в один влезает 4 штуки

bool treeFileOutput(const char* fileName) {
    std::ofstream fileStream(fileName, std::ios::out | std::ios::binary);
    if (!fileStream.is_open()) {
        return 1;
    }
    fileStream << treeDepth;
    const int _SHIFT = 4;
    for (int i = 0; i < tree.size(); i += _SHIFT) {
        unsigned char add = 0;
        for (int j = i; j < std::min(i + _SHIFT, int(tree.size())); ++j) {
            add <<= 1;
            add |= tree[i].draw;
            add <<= 1;
            add |= tree[i].stop;
        }
        fileStream << add;
    }
    fileStream.close();
    return false;
}

bool treeFileInput(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (!fileStream.is_open()) {
        return 1;
    }
    fileStream >> treeDepth;
    initTree(drawCube, treeDepth);
    const int _SHIFT = 4;
    for (int i = 0; i < tree.size(); i += _SHIFT) {
        unsigned char cur;
        fileStream >> cur;
        for (int j = std::min(int(tree.size()) - 1, i + _SHIFT - 1); j >= i; --j) {
            tree[i].stop = (cur & 1);
            cur >>= 1;
            tree[i].draw = (cur & 1);
            cur >>= 1;
        }
    }
    fileStream.close();
    return false;
}
