#pragma once

namespace Real3D {
    class World;
    class AABB;
    class Player;

    class Chunk {
    public:
        World* world;
        int x0;
        int y0;
        int z0;
        int x1;
        int y1;
        int z1;
        double x;
        double y;
        double z;
        AABB* aabb;
        double dirtiedTime;
    private:
        bool dirty;
        int lists;
    public:
        Chunk(World* _world,
            int _x0,
            int _y0,
            int _z0,
            int _x1,
            int _y1,
            int _z1);
        ~Chunk();
        void rebuild(int layer);
        void rebuild();
        void render(int layer);
        void markDirty();
        bool isDirty();
        double distanceToSqr(Player* player);
    };
}
