#pragma once
#include <vector>
#include "frustum.h"
#include "glad/gl.h"

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
        Chunk** chunks;
        int xChunks;
        int yChunks;
        int zChunks;
        int chunksSize;
    public:
        static constexpr int MAX_REBUILDS_PER_FRAME = 8;
        static constexpr int CHUNK_SIZE = 16;
        WorldRenderer(World* _world);
        ~WorldRenderer();
        /// <summary>
        /// Get all dirty chunks.
        /// </summary>
        /// <returns>A vector on heap or nullptr. You must delete it.</returns>
        std::vector<Chunk*>* getAllDirtyChunks();
        void render(GLuint blockAtlas);
        void updateDirtyChunks(Player* player);
        void pick(Player* player, Frustum& frustum);
        void renderHit(HitResult* h);
        void setDirty(int x0, int y0, int z0, int x1, int y1, int z1);
        void blockChanged(int x, int y, int z) override;
        void lightColumnChanged(int x, int z, int y0, int y1) override;
        void allChanged() override;
    };
}
