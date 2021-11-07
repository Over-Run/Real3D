#pragma once

namespace Real3D {
    class AABB {
    private:
        double epsilon = 0.0;
    public:
        static AABB* FULL_CUBE;
        double x0;
        double y0;
        double z0;
        double x1;
        double y1;
        double z1;

        AABB(double _x0,
            double _y0,
            double _z0,
            double _x1,
            double _y1,
            double _z1);
        AABB expand(double xa,  double ya, double za);
        AABB grow(double xa, double ya, double za);
        double clipXCollide(AABB c, double xa);
        double clipYCollide(AABB c, double ya);
        double clipZCollide(AABB c, double za);
        bool intersects(AABB c);
        void move(double xa, double ya, double za);
    };
}
