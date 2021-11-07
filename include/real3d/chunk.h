#pragma once

namespace Real3D {
    class AABB;
    class World;
    class Player;

    class Chunk {
    public:
        static int updates;
        static double totalTime;
        static int totalUpdates;

        AABB* aabb;
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
    private:
        bool dirty;
        int list;
        double dirtiedTime;
    public:
        Chunk(World* _world,
            int _x0,
            int _y0,
            int _z0,
            int _x1,
            int _y1,
            int _z1);
        ~Chunk();
        void rebuild();
        void render();
        void setDirty();
        bool isDirty();
        double distanceToSqr(Player* player);
        bool operator<(const Chunk& c1);
    };
}
