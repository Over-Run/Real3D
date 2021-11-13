#include "real3d/client/chunk.h"
#include "real3d/world.h"

using Real3D::Chunk;

Chunk::Chunk(World* _world,
    int _x0,
    int _y0,
    int _z0,
    int _x1,
    int _y1,
    int _z1) :
    world(_world),
    x0(_x0),
    y0(_y0),
    z0(_z0),
    x1(_x1),
    y1(_y1),
    z1(_z1),
    x((_x0 + _x1) / 2.0),
    y((_y0 + _y1) / 2.0),
    z((_z0 + _z1) / 2.0),
    aabb(new AABB(_x0, _y0, _z0, _x1, _y1, _z1)),
    dirty(true),
    list(glGenLists(1)) {}
Chunk::~Chunk() {
    delete aabb;
    glDeleteLists(list, 1);
}
void Chunk::rebuild() {
    dirty = false;

    glNewList(list, GL_COMPILE);
    //glBegin(GL_QUADS);

    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Block* block = world->getBlock(x, y, z);
                if (block != Blocks::AIR) {
                    block->render(world, x, y, z);
                }
            }
        }
    }

    //glEnd();
    glEndList();
}
void Chunk::render() {
    glCallList(list);
}
void Chunk::markDirty() {
    dirty = true;
}
bool Chunk::isDirty() {
    return dirty;
}
double Chunk::distanceToSqr(Player* player) {
    double xd = player->x - x;
    double yd = player->y - y;
    double zd = player->z - z;
    return xd * xd + yd * yd + zd * zd;
}
