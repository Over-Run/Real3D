#pragma once
#include <vector>

namespace Real3D {
    class Block;
    class WorldListener;
    class AABB;
    class Player;

    class World {
    public:
        static constexpr int width = 16 * 1;
        static constexpr int height = 16 * 1;
        static constexpr int depth = 16 * 1;
        Block* blocks[width * height * depth];
        int lightDepths[width * depth];
        std::vector<WorldListener*> listeners;
        unsigned int random;
        int unprocessed;

        World();
        void calcLightDepths(int x0, int z0, int x1, int z1);
        void addListener(WorldListener* listener);
        bool isLightBlocker(int x, int y, int z);
        std::vector<AABB*> getCubes(AABB* aabb);
        bool isLit(int x, int y, int z);
        bool isSolidTile(int x, int y, int z);
        int getIndex(int x, int y, int z);
        bool inBorder(int x, int y, int z);
        void setBlock(int x, int y, int z, Block* block);
        Block* getBlock(int x, int y, int z);
        void randomTick(Player* player);
    };
}
