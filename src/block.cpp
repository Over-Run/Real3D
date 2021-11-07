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
void Block::pickFace(int x, int y, int z, Direction dir) {
    auto x0 = x;
    auto x1 = 1 + x;
    auto y0 = y;
    auto y1 = 1 + y;
    auto z0 = z;
    auto z1 = 1 + z;
    switch (dir) {
    case Direction::NORTH:
        glVertex3d(x1, y0, z0);
        glVertex3d(x0, y0, z0);
        glVertex3d(x0, y1, z0);
        glVertex3d(x1, y1, z0);
        break;
    case Direction::SOUTH:
        glVertex3d(x0, y0, z1);
        glVertex3d(x1, y0, z1);
        glVertex3d(x1, y1, z1);
        glVertex3d(x0, y1, z1);
        break;
    case Direction::WEST:
        glVertex3d(x0, y0, z0);
        glVertex3d(x0, y0, z1);
        glVertex3d(x0, y1, z1);
        glVertex3d(x0, y1, z0);
        break;
    case Direction::EAST:
        glVertex3d(x1, y0, z1);
        glVertex3d(x1, y0, z0);
        glVertex3d(x1, y1, z0);
        glVertex3d(x1, y1, z1);
        break;
    case Direction::UP: {
        glVertex3d(x0, y1, z1);
        glVertex3d(x1, y1, z1);
        glVertex3d(x1, y1, z0);
        glVertex3d(x0, y1, z0);
        break;
    }
    case Direction::DOWN:
        glVertex3d(x0, y0, z0);
        glVertex3d(x1, y0, z0);
        glVertex3d(x1, y0, z1);
        glVertex3d(x0, y0, z1);
        break;
    }
}
void Block::renderFace(int x, int y, int z, Direction dir) {
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
        glTexCoord2d(su0, sv1); glVertex3d(x1, y0, z0);
        glTexCoord2d(su1, sv1); glVertex3d(x0, y0, z0);
        glTexCoord2d(su1, sv0); glVertex3d(x0, y1, z0);
        glTexCoord2d(su0, sv0); glVertex3d(x1, y1, z0);
        break;
    case Direction::SOUTH:
        glTexCoord2d(su0, sv1); glVertex3d(x0, y0, z1);
        glTexCoord2d(su1, sv1); glVertex3d(x1, y0, z1);
        glTexCoord2d(su1, sv0); glVertex3d(x1, y1, z1);
        glTexCoord2d(su0, sv0); glVertex3d(x0, y1, z1);
        break;
    case Direction::WEST:
        glTexCoord2d(su0, sv1); glVertex3d(x0, y0, z0);
        glTexCoord2d(su1, sv1); glVertex3d(x0, y0, z1);
        glTexCoord2d(su1, sv0); glVertex3d(x0, y1, z1);
        glTexCoord2d(su0, sv0); glVertex3d(x0, y1, z0);
        break;
    case Direction::EAST:
        glTexCoord2d(su0, sv1); glVertex3d(x1, y0, z1);
        glTexCoord2d(su1, sv1); glVertex3d(x1, y0, z0);
        glTexCoord2d(su1, sv0); glVertex3d(x1, y1, z0);
        glTexCoord2d(su0, sv0); glVertex3d(x1, y1, z1);
        break;
    case Direction::UP: {
        auto tu0 = (textureTop % 16) * 16.0 / 256.0;
        auto tu1 = ((textureTop % 16) + 1) * 16.0 / 256.0;
        auto tv0 = textureTop / 16 / 256.0;
        auto tv1 = (textureTop / 16 + 16) / 256.0;
        glTexCoord2d(tu0, tv1); glVertex3d(x0, y1, z1);
        glTexCoord2d(tu1, tv1); glVertex3d(x1, y1, z1);
        glTexCoord2d(tu1, tv0); glVertex3d(x1, y1, z0);
        glTexCoord2d(tu0, tv0); glVertex3d(x0, y1, z0);
        break;
    }
    case Direction::DOWN:
        auto bu0 = (textureBottom % 16) * 16.0 / 256.0;
        auto bu1 = ((textureBottom % 16) + 1) * 16.0 / 256.0;
        auto bv0 = textureBottom / 16 / 256.0;
        auto bv1 = (textureBottom / 16 + 16) / 256.0;
        glTexCoord2d(bu0, bv1); glVertex3d(x0, y0, z0);
        glTexCoord2d(bu1, bv1); glVertex3d(x1, y0, z0);
        glTexCoord2d(bu1, bv0); glVertex3d(x1, y0, z1);
        glTexCoord2d(bu0, bv0); glVertex3d(x0, y0, z1);
        break;
    }
}
void Block::render(World* world, int x, int y, int z) {
    glColor3d(1, 1, 1);
    if (world->getBlock(x, y, z - 1) == Blocks::AIR) {
        renderFace(x, y, z, Direction::NORTH);
    }
    if (world->getBlock(x, y, z + 1) == Blocks::AIR) {
        renderFace(x, y, z, Direction::SOUTH);
    }
    if (world->getBlock(x - 1, y, z) == Blocks::AIR) {
        renderFace(x, y, z, Direction::WEST);
    }
    if (world->getBlock(x + 1, y, z) == Blocks::AIR) {
        renderFace(x, y, z, Direction::EAST);
    }
    if (world->getBlock(x, y + 1, z) == Blocks::AIR) {
        renderFace(x, y, z, Direction::UP);
    }
    if (world->getBlock(x, y - 1, z) == Blocks::AIR) {
        renderFace(x, y, z, Direction::DOWN);
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
void AirBlock::pickFace(int, int, int, Direction) {}
void AirBlock::renderFace(int, int, int, Direction) {}
void AirBlock::render(World*, int, int, int) {}

void Blocks::init() {
    GRASS_BLOCK->textureTop = 0;
    GRASS_BLOCK->textureBottom = 3;
}
Block* Blocks::AIR = new AirBlock(0);
Block* Blocks::STONE = new Block(1);
Block* Blocks::GRASS_BLOCK = new Block(2);
