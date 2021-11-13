#pragma once
#include "glad/gl.h"
#include "enums.h"
#include "client/tesselator.h"

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

        virtual bool isSolid();
        virtual bool isOpaque();
        virtual bool isAir();
        virtual AABB* getOutline();
        virtual AABB* getCollision();
        virtual void pickFace(Tesselator& t, int x, int y, int z, Direction dir);
        virtual void renderFace(Tesselator& t, int x, int y, int z, Direction dir);
        virtual void render(Tesselator& t, World* world, int x, int y, int z);
    };

    class AirBlock : public Block {
    public:
        AirBlock(int _id);

        bool isSolid() override;
        bool isOpaque() override;
        bool isAir() override;
        AABB* getOutline() override;
        AABB* getCollision() override;
        void pickFace(Tesselator&, int, int, int, Direction) override;
        void renderFace(Tesselator& t, int, int, int, Direction) override;
        void render(Tesselator& t, World*, int, int, int) override;
    };

    struct Blocks {
        static Block* AIR;
        static Block* STONE;
        static Block* GRASS_BLOCK;

        static void init();
    };
}
