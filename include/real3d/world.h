#pragma once
#include <vector>
#include "block.h"
#include "aabb.h"

namespace Real3D {
    class World {
    public:
        static constexpr int width = 16;
        static constexpr int height = 16;
        static constexpr int depth = 16;
        Block* blocks[width * height * depth];

        World() {
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    for (int z = 0; z < depth; ++z) {
                        Block* b = Blocks::AIR;
                        if (y < 4) {
                            b = Blocks::STONE;
                        }
                        else if (y == 4) {
                            b = Blocks::GRASS_BLOCK;
                        }
                        setBlock(x, y, z, b);
                    }
                }
            }
        }

        void render(GLuint blockAtlas) {
            glBindTexture(GL_TEXTURE_2D, blockAtlas);

            glBegin(GL_QUADS);
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    for (int z = 0; z < depth; ++z) {
                        getBlock(x, y, z)->render(this, x, y, z);
                    }
                }
            }
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        std::vector<AABB*> getCubes(AABB aabb) {
            std::vector<AABB*> cubes;
            int x0 = (int)aabb.x0;
            int x1 = (int)(aabb.x1 + 1);
            int y0 = (int)aabb.y0;
            int y1 = (int)(aabb.y1 + 1);
            int z0 = (int)aabb.z0;
            int z1 = (int)(aabb.z1 + 1);
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

        int getIndex(int x, int y, int z) {
            return x + y * width + z * width * height;
        }

        bool inBorder(int x, int y, int z) {
            return x >= 0 && x < width
                && y >= 0 && y < height
                && z >= 0 && z < depth;
        }

        void setBlock(int x, int y, int z, Block* block) {
            if (inBorder(x, y, z))
                blocks[getIndex(x, y, z)] = block;
        }

        Block* getBlock(int x, int y, int z) {
            if (inBorder(x, y, z))
                return blocks[getIndex(x, y, z)];
            return Blocks::AIR;
        }
    };
}
