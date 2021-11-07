#pragma once
#include "glad/gl.h"
#include "enums.h"

namespace Real3D {
    class World;
    class AABB;

    class Block {
    public:
        const int id;
        int textureTop;
        int textureSide;
        int textureBottom;
        Block(int _id);

        virtual AABB* getCollision();
        virtual void renderFace(int x, int y, int z, Direction dir);
        virtual void render(World* world, int x, int y, int z);
    };

    class AirBlock : public Block {
    public:
        AirBlock(int _id);

        AABB* getCollision() override;
        void renderFace(int, int, int, Direction) override;
        void render(World*, int, int, int) override;
    };

    struct Blocks {
        static Block* AIR;
        static Block* STONE;
        static Block* GRASS_BLOCK;

        static void init();
    };
}
