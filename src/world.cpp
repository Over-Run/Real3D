#include <ctime>
#include "real3d/world.h"
#include "real3d/block.h"
#include "real3d/player.h"
#include "real3d/client/world_renderer.h"

using Real3D::World;
using Real3D::AABB;
using Real3D::Block;
using std::vector;

World::World() :
    random(time(nullptr)),
    unprocessed(0) {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                Block* b = Blocks::AIR;
                if (y < 10) {
                    b = Blocks::STONE;
                }
                else if (y < 15) {
                    b = Blocks::DIRT;
                }
                else if (y == 15) {
                    b = Blocks::GRASS_BLOCK;
                }
                blocks[getIndex(x, y, z)] = b;
            }
        }
    }
    calcLightDepths(0, 0, width, depth);
}
void World::calcLightDepths(int x0, int z0, int x1, int z1) {
    for (int x = x0; x < x0 + x1; ++x) {
        for (int z = z0; z < z0 + z1; ++z) {
            int oldDepth = lightDepths[x + z * width];

            int y = height - 1;
            while (y > 0 && !isLightBlocker(x, y, z)) {
                --y;
            }

            lightDepths[x + z * width] = y;
            if (oldDepth != y) {
                int yl0 = fmin(oldDepth, y);
                int yl1 = fmax(oldDepth, y);
                for (auto& listener : listeners) {
                    listener->lightColumnChanged(x, z, yl0, yl1);
                }
            }
        }
    }
}
void World::addListener(WorldListener* listener) {
    listeners.push_back(listener);
}
bool World::isLightBlocker(int x, int y, int z) {
    return getBlock(x, y, z)->isOpaque();
}
vector<AABB*> World::getCubes(AABB* aabb) {
    std::vector<AABB*> cubes;
    int x0 = (int)aabb->x0;
    int x1 = (int)(aabb->x1 + 1);
    int y0 = (int)aabb->y0;
    int y1 = (int)(aabb->y1 + 1);
    int z0 = (int)aabb->z0;
    int z1 = (int)(aabb->z1 + 1);
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (z0 < 0) {
        z0 = 0;
    }
    if (x1 > width) {
        x1 = width;
    }
    if (y1 > height) {
        y1 = height;
    }
    if (z1 > depth) {
        z1 = depth;
    }
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                AABB* aabb = getBlock(x, y, z)->getCollision();
                if (aabb != nullptr) {
                    AABB* cpy = new AABB(*aabb);
                    cpy->move(x, y, z);
                    cubes.push_back(cpy);
                }
            }
        }
    }
    return cubes;
}
bool World::isLit(int x, int y, int z) {
    return !inBorder(x, y, z) || y >= lightDepths[x + z * width];
}
bool World::isSolidTile(int x, int y, int z) {
    return getBlock(x, y, z)->isSolid();
}
int World::getIndex(int x, int y, int z) {
    return x + y * width + z * width * height;
}
bool World::inBorder(int x, int y, int z) {
    return x >= 0 && x < width
        && y >= 0 && y < height
        && z >= 0 && z < depth;
}
void World::setBlock(int x, int y, int z, Block* block) {
    if (inBorder(x, y, z)) {
        int i = getIndex(x, y, z);
        if (blocks[i] == block) {
            return;
        }
        blocks[i] = block;
        calcLightDepths(x, z, 1, 1);
        for (auto& l : listeners) {
            l->blockChanged(x, y, z);
        }
    }
}
Block* World::getBlock(int x, int y, int z) {
    if (inBorder(x, y, z))
        return blocks[getIndex(x, y, z)];
    return Blocks::AIR;
}
void World::randomTick(Player* player) {
    srand(random);
    /// Updating radius
    /// See also: client/world_renderer.cpp
    constexpr int r = 12;
    constexpr auto CHUNK_SIZE = WorldRenderer::CHUNK_SIZE;
    constexpr auto c = r * CHUNK_SIZE;
    int x0 = player->x - c;
    int y0 = player->y - c;
    int z0 = player->z - c;
    int x1 = player->x + c;
    int y1 = player->y + c;
    int z1 = player->z + c;
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (z0 < 0) {
        z0 = 0;
    }
    if (x1 > width) {
        x1 = width;
    }
    if (y1 > height) {
        y1 = height;
    }
    if (z1 > depth) {
        z1 = depth;
    }
    int width = x1 - x0;
    int height = y1 - y0;
    int depth = z1 - z0;
    unprocessed += width * height * depth;
    int ticks = unprocessed / 400;
    unprocessed -= ticks * 400;
    for (int i = 0; i < ticks; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        int z = rand() % depth;
        getBlock(x, y, z)->tick(this, player, x, y, z, random);
    }
}
