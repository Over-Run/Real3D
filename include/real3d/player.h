#pragma once
#include "real3d/aabb.h"

namespace Real3D {
    class Player {
    public:
        double prevX = 0, prevY = 0, prevZ = 0;
        double x = 0, y = 0, z = 0;
        double xd = 0, yd = 0, zd = 0;
        double xRot = 0, yRot = 0;
        bool onGround = false;
        double bbWidth = 0.6;
        double bbHeight = 1.8;
        double heightOffset = 1.62;
        AABB* bb;

        Player();

        void tick();
        void move(double xa, double ya, double za);
        void moveRelative(double x, double y, double z);
        void turn(double xr, double yr);
    };
}
