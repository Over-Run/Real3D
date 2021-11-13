#pragma once
#include <unordered_map>
#include "frustum.h"
#include "glad/gl.h"
#include "../pos.h"

namespace Real3D {
    class World;
    class Chunk;
    class Player;
    class HitResult;

    class WorldListener {
    public:
        virtual void blockChanged(int x, int y, int z) = 0;
        virtual void lightColumnChanged(int x, int z, int y0, int y1) = 0;
        virtual void allChanged() = 0;
    };

    class WorldRenderer : public WorldListener {
    private:
        World* world;
        /// <summary>Loaded chunks.</summary>
        std::unordered_map<BlockPos, Chunk*> chunks;
        int xChunks;
        int yChunks;
        int zChunks;
    public:
        static constexpr int MAX_REBUILDS_PER_FRAME = 8;
        static constexpr int CHUNK_SIZE = 16;
        WorldRenderer(World* _world);
        ~WorldRenderer();
        /// <summary>
        /// Get all dirty chunks
        /// </summary>
        /// <returns>A pointer of the vector. You must delete it.</returns>
        std::vector<Chunk*>* getAllDirtyChunks();
        void updateDirtyChunks(Player* player);
        void pick(Player* player, Frustum& frustum);
        void render(Player* player, int layer, GLuint blockAtlas);
        void renderHit(HitResult* h);
        void markDirty(int x0, int y0, int z0, int x1, int y1, int z1);
        void blockChanged(int x, int y, int z) override;
        void lightColumnChanged(int x, int z, int y0, int y1) override;
        void allChanged() override;
    };
}
