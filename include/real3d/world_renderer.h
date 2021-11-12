#pragma once
#include "frustum.h"
#include "glad/gl.h"

namespace Real3D {
    class World;
    class Player;
    class HitResult;

    class WorldRenderer {
    private:
        World* world;
    public:
        static constexpr int CHUNK_SIZE = 16;
        WorldRenderer(World* _world);
        void pick(Player* player, Frustum& frustum);
        void render(GLuint blockAtlas);
        void renderHit(HitResult* h);
    };
}
