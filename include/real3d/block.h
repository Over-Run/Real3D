#pragma once
#include "glad/gl.h"

namespace Real3D {
    class Block {
    public:
        const int id;
        int textureTop = id;
        int textureSide = id;
        int textureBottom = id;
        Block(int _id) : id(_id) {}

        void render(int x, int y, int z) {
            auto su0 = (textureSide % 16) * 16.0 / 256.0;
            auto su1 = ((textureSide % 16) + 1) * 16.0 / 256.0;
            auto sv0 = textureSide / 16 / 256.0;
            auto sv1 = (textureSide / 16 + 16) / 256.0;
            auto tu0 = (textureTop % 16) * 16.0 / 256.0;
            auto tu1 = ((textureTop % 16) + 1) * 16.0 / 256.0;
            auto tv0 = textureTop / 16 / 256.0;
            auto tv1 = (textureTop / 16 + 16) / 256.0;
            auto bu0 = (textureBottom % 16) * 16.0 / 256.0;
            auto bu1 = ((textureBottom % 16) + 1) * 16.0 / 256.0;
            auto bv0 = textureBottom / 16 / 256.0;
            auto bv1 = (textureBottom / 16 + 16) / 256.0;
            auto x0 = x;
            auto x1 = 1 + x;
            auto y0 = y;
            auto y1 = 1 + y;
            auto z0 = z;
            auto z1 = 1 + z;
            // North
            glTexCoord2d(su0, sv1); glVertex3d(x1, y0, z0);
            glTexCoord2d(su1, sv1); glVertex3d(x0, y0, z0);
            glTexCoord2d(su1, sv0); glVertex3d(x0, y1, z0);
            glTexCoord2d(su0, sv0); glVertex3d(x1, y1, z0);
            // South
            glTexCoord2d(su0, sv1); glVertex3d(x0, y0, z1);
            glTexCoord2d(su1, sv1); glVertex3d(x1, y0, z1);
            glTexCoord2d(su1, sv0); glVertex3d(x1, y1, z1);
            glTexCoord2d(su0, sv0); glVertex3d(x0, y1, z1);
            // West
            glTexCoord2d(su0, sv1); glVertex3d(x0, y0, z0);
            glTexCoord2d(su1, sv1); glVertex3d(x0, y0, z1);
            glTexCoord2d(su1, sv0); glVertex3d(x0, y1, z1);
            glTexCoord2d(su0, sv0); glVertex3d(x0, y1, z0);
            // East
            glTexCoord2d(su0, sv1); glVertex3d(x1, y0, z1);
            glTexCoord2d(su1, sv1); glVertex3d(x1, y0, z0);
            glTexCoord2d(su1, sv0); glVertex3d(x1, y1, z0);
            glTexCoord2d(su0, sv0); glVertex3d(x1, y1, z1);
            // Up
            glTexCoord2d(tu0, tv1); glVertex3d(x0, y1, z1);
            glTexCoord2d(tu1, tv1); glVertex3d(x1, y1, z1);
            glTexCoord2d(tu1, tv0); glVertex3d(x1, y1, z0);
            glTexCoord2d(tu0, tv0); glVertex3d(x0, y1, z0);
            // Down
            glTexCoord2d(bu0, bv1); glVertex3d(x0, y0, z0);
            glTexCoord2d(bu1, bv1); glVertex3d(x1, y0, z0);
            glTexCoord2d(bu1, bv0); glVertex3d(x1, y0, z1);
            glTexCoord2d(bu0, bv0); glVertex3d(x0, y0, z1);
        }
    };

    struct Blocks {
        static Block* STONE;
        static Block* GRASS_BLOCK;

        static void init() {
            GRASS_BLOCK->textureTop = 0;
            GRASS_BLOCK->textureBottom = 3;
        }
    };

    Block* Blocks::STONE = new Block(1);
    Block* Blocks::GRASS_BLOCK = new Block(2);
}
