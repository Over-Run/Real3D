#include "real3d/chunk.h"
#include "real3d/world.h"
#include "real3d/timer.h"
#include "GLFW/glfw3.h"

using Real3D::Chunk;
using Real3D::World;
using Real3D::Player;

extern Player* player;

int Chunk::updates = 0;
double Chunk::totalTime = 0.0;
int Chunk::totalUpdates = 0;
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
    x((x0 + x1) / 2.0),
    y((y0 + y1) / 2.0),
    z((z0 + z1) / 2.0),
    aabb(new AABB(x0, y0, z0, x1, y1, z1)),
    list(glGenLists(1)),
    dirty(true),
    dirtiedTime(0.0) {}
Chunk::~Chunk() {
    delete aabb;
    glDeleteLists(list, 1);
}
void Chunk::rebuild() {
    dirty = false;
    ++updates;
    double before = glfwGetTime() * NS_PER_SECOND;

    glNewList(list, GL_COMPILE);
    glBegin(GL_QUADS);
    int blocks = 0;
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                Block* block = world->getBlock(x, y, z);
                if (block != Blocks::AIR) {
                    block->render(world, x, y, z);
                    ++blocks;
                }
            }
        }
    }
    glEnd();
    glEndList();

    long after = glfwGetTime() * NS_PER_SECOND;
    if (blocks > 0) {
        totalTime += after - before;
        ++totalUpdates;
    }
}
void Chunk::render() {
    glCallList(list);
}
void Chunk::setDirty() {
    if (!dirty) {
        dirtiedTime = glfwGetTime() * 1000;
    }
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
bool Chunk::operator<(const Chunk& c1) {
    Frustum& frustum = Frustum::getFrustum();
    double now = glfwGetTime() * 1000;
    bool i0 = frustum.isVisible(aabb);
    bool i1 = frustum.isVisible(c1.aabb);
    if (i0 && !i1) {
        return false;
    }
    else if (i1 && !i0) {
        return true;
    }
    else {
        int t0 = (int)((now - dirtiedTime) / 2000L);
        int t1 = (int)((now - c1.dirtiedTime) / 2000L);
        if (t0 < t1) {
            return false;
        }
        else if (t0 > t1) {
            return true;
        }
        else {
            return distanceToSqr(player) < const_cast<Chunk&>(c1).distanceToSqr(player) ? -1 : 1;
        }
    }
}
