#include "real3d/block.h"
#include "real3d/world.h"

using Real3D::AABB;
using Real3D::Block;
using Real3D::AirBlock;
using Real3D::Blocks;
using Real3D::Direction;
using Real3D::World;

Block::Block(int _id) :
    id(_id),
    textureTop(_id),
    textureSide(_id),
    textureBottom(_id) {}
bool Block::isSolid() {
    return true;
}
bool Block::isOpacity() {
    return true;
}
AABB* Block::getOutline() {
    return AABB::FULL_CUBE;
}
AABB* Block::getCollision() {
    return AABB::FULL_CUBE;
}
void Block::pickFace(Tesselator& t, int x, int y, int z, Direction dir) {
    auto x0 = x;
    auto x1 = 1 + x;
    auto y0 = y;
    auto y1 = 1 + y;
    auto z0 = z;
    auto z1 = 1 + z;
    switch (dir) {
    case Direction::NORTH:
        t.vertex(x1, y0, z0)
            .vertex(x0, y0, z0)
            .vertex(x0, y1, z0)
            .vertex(x1, y1, z0);
        break;
    case Direction::SOUTH:
        t.vertex(x0, y0, z1)
            .vertex(x1, y0, z1)
            .vertex(x1, y1, z1)
            .vertex(x0, y1, z1);
        break;
    case Direction::WEST:
        t.vertex(x0, y0, z0)
            .vertex(x0, y0, z1)
            .vertex(x0, y1, z1)
            .vertex(x0, y1, z0);
        break;
    case Direction::EAST:
        t.vertex(x1, y0, z1)
            .vertex(x1, y0, z0)
            .vertex(x1, y1, z0)
            .vertex(x1, y1, z1);
        break;
    case Direction::UP:
        t.vertex(x0, y1, z1)
            .vertex(x1, y1, z1)
            .vertex(x1, y1, z0)
            .vertex(x0, y1, z0);
        break;
    case Direction::DOWN:
        t.vertex(x0, y0, z0)
            .vertex(x1, y0, z0)
            .vertex(x1, y0, z1)
            .vertex(x0, y0, z1);
        break;
    }
}
void Block::renderFace(Tesselator& t, int x, int y, int z, Direction dir) {
    auto su0 = (textureSide % 16) * 16.0 / 256.0;
    auto su1 = ((textureSide % 16) + 1) * 16.0 / 256.0;
    auto sv0 = textureSide / 16 / 256.0;
    auto sv1 = (textureSide / 16 + 16) / 256.0;
    auto x0 = x;
    auto x1 = 1 + x;
    auto y0 = y;
    auto y1 = 1 + y;
    auto z0 = z;
    auto z1 = 1 + z;
    switch (dir) {
    case Direction::NORTH:
        t.vertexUV(x1, y0, z0, su0, sv1)
            .vertexUV(x0, y0, z0, su1, sv1)
            .vertexUV(x0, y1, z0, su1, sv0)
            .vertexUV(x1, y1, z0, su0, sv0);
        break;
    case Direction::SOUTH:
        t.vertexUV(x0, y0, z1, su0, sv1)
            .vertexUV(x1, y0, z1, su1, sv1)
            .vertexUV(x1, y1, z1, su1, sv0)
            .vertexUV(x0, y1, z1, su0, sv0);
        break;
    case Direction::WEST:
        t.vertexUV(x0, y0, z0, su0, sv1)
            .vertexUV(x0, y0, z1, su1, sv1)
            .vertexUV(x0, y1, z1, su1, sv0)
            .vertexUV(x0, y1, z0, su0, sv0);
        break;
    case Direction::EAST:
        t.vertexUV(x1, y0, z1, su0, sv1)
            .vertexUV(x1, y0, z0, su1, sv1)
            .vertexUV(x1, y1, z0, su1, sv0)
            .vertexUV(x1, y1, z1, su0, sv0);
        break;
    case Direction::UP: {
        auto tu0 = (textureTop % 16) * 16.0 / 256.0;
        auto tu1 = ((textureTop % 16) + 1) * 16.0 / 256.0;
        auto tv0 = textureTop / 16 / 256.0;
        auto tv1 = (textureTop / 16 + 16) / 256.0;
        t.vertexUV(x0, y1, z1, tu0, tv1)
            .vertexUV(x1, y1, z1, tu1, tv1)
            .vertexUV(x1, y1, z0, tu1, tv0)
            .vertexUV(x0, y1, z0, tu0, tv0);
        break;
    }
    case Direction::DOWN:
        auto bu0 = (textureBottom % 16) * 16.0 / 256.0;
        auto bu1 = ((textureBottom % 16) + 1) * 16.0 / 256.0;
        auto bv0 = textureBottom / 16 / 256.0;
        auto bv1 = (textureBottom / 16 + 16) / 256.0;
        t.vertexUV(x0, y0, z0, bu0, bv1)
            .vertexUV(x1, y0, z0, bu1, bv1)
            .vertexUV(x1, y0, z1, bu1, bv0)
            .vertexUV(x0, y0, z1, bu0, bv0);
        break;
    }
}
void Block::render(Tesselator& t, World* world, int x, int y, int z) {
    float c1 = 1.0f;
    float c2 = 0.8f;
    float c3 = 0.6f;
    if (world->getBlock(x, y, z - 1) == Blocks::AIR) {
        t.color(c3, c3, c3);
        renderFace(t, x, y, z, Direction::NORTH);
    }
    if (world->getBlock(x, y, z + 1) == Blocks::AIR) {
        t.color(c3, c3, c3);
        renderFace(t, x, y, z, Direction::SOUTH);
    }
    if (world->getBlock(x - 1, y, z) == Blocks::AIR) {
        t.color(c1, c1, c1);
        renderFace(t, x, y, z, Direction::WEST);
    }
    if (world->getBlock(x + 1, y, z) == Blocks::AIR) {
        t.color(c1, c1, c1);
        renderFace(t, x, y, z, Direction::EAST);
    }
    if (world->getBlock(x, y + 1, z) == Blocks::AIR) {
        t.color(c2, c2, c2);
        renderFace(t, x, y, z, Direction::UP);
    }
    if (world->getBlock(x, y - 1, z) == Blocks::AIR) {
        t.color(c2, c2, c2);
        renderFace(t, x, y, z, Direction::DOWN);
    }
}

AirBlock::AirBlock(int _id) : Block(_id) {}
bool AirBlock::isSolid() {
    return false;
}
bool AirBlock::isOpacity() {
    return false;
}
AABB* AirBlock::getOutline() {
    return nullptr;
}
AABB* AirBlock::getCollision() {
    return nullptr;
}
void AirBlock::pickFace(Tesselator&, int, int, int, Direction) {}
void AirBlock::renderFace(Tesselator&, int, int, int, Direction) {}
void AirBlock::render(Tesselator&, World*, int, int, int) {}

void Blocks::init() {
    GRASS_BLOCK->textureTop = 0;
    GRASS_BLOCK->textureBottom = 3;
}
Block* Blocks::AIR = new AirBlock(0);
Block* Blocks::STONE = new Block(1);
Block* Blocks::GRASS_BLOCK = new Block(2);
