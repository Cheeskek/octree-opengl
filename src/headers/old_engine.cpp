#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <vector>

namespace {
    int worldSize = 256;
    enum {
        NODE_STOP,
        NODE_DRAW
    };
    struct Node {
        bool stop : 1;
        bool draw : 1;
        Node() : stop(false), draw(false) {}
    };
    std::vector<Node> tree;
    int treeDepth;
    void (*drawCube) (int, int, int, int);
}

int getScale(int depth) {
    return (worldSize >> depth);
}

void initTree(void (*drawCubeFunction) (int, int, int, int), int depth = 4) {
    treeDepth = depth;
    int siz = 0;
    for (int i = 0; i <= depth; ++i) {
        siz += (1 << (i * 3));
    }
    tree.resize(siz);
    drawCube = drawCubeFunction;
    tree[0].stop = true;
}

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

// TODO
// обнови функцию update. она криво работает. отвратительно (ПОЛНОСТЬЮ ПЕРЕПИШИ ПРОСТО, НЕВОЗМОЖНО НОРМАЛЬНО РАБОТАТЬ)
// мультипоточность, мультипоточность, надо бы добавить (лучше бы вообще на gpu переписать)
//      короче, что про мультипоточность, нужно сам движок на отдельном треде запустить и сохранить что рисовять в очередь какую-нибудь
//      конечно, это ничего шибко не поменяет, но сойдёт
// что с размерами мира? почему там, где можно рисовать съезжает в сторону? почему кубы можно ставить с расстоянием в половину?
// ну и пока всё, код выкладывать на гитхаб не буду, не хочу.

void update(int x, int y, int z, int scale, bool add,
        int cur = 0, int _x = 0, int _y = 0, int _z = 0, int _depth = 0) {
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
    // чтобы не выйти за границы
    if (_depth == treeDepth) {
        return;
    }
    // пуш вниз по дереву, чтобы не удалить лишнего
    if (tree[cur].stop) {
        if (tree[cur].draw != add) {
            for (int i = 1; i <= 8; ++i) {
                tree[(cur << 3) + i].draw = tree[cur].draw;
            }
            tree[cur].draw = false;
            tree[cur].stop = false;
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
            tree[cur].stop = true;
        }
        else if (ans == 0) {
            tree[cur].draw = false;
            tree[cur].stop = true;
        }
    }
}

#ifdef _DEBUG

void _debugTreeOutput() {
    printf("ans - %d\n", tree[0].stop);
    fflush(stdout);
}

#endif
