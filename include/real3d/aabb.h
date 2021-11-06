#pragma once

namespace Real3D {
    class AABB {
    private:
        double epsilon = 0.0;
    public:
        double x0;
        double y0;
        double z0;
        double x1;
        double y1;
        double z1;

        AABB(double _x0, double _y0, double _z0, double _x1, double _y1, double _z1) :
            x0(_x0),
            y0(_y0),
            z0(_y0),
            x1(_x1),
            y1(_y1),
            z1(_z1) {
        }

        AABB expand(double xa, double ya, double za) {
            double _x0 = x0;
            double _y0 = y0;
            double _z0 = z0;
            double _x1 = x1;
            double _y1 = y1;
            double _z1 = z1;
            if (xa < 0.0F) {
                _x0 += xa;
            }

            if (xa > 0.0F) {
                _x1 += xa;
            }

            if (ya < 0.0F) {
                _y0 += ya;
            }

            if (ya > 0.0F) {
                _y1 += ya;
            }

            if (za < 0.0F) {
                _z0 += za;
            }

            if (za > 0.0F) {
                _z1 += za;
            }

            return AABB(_x0, _y0, _z0, _x1, _y1, _z1);
        }

        AABB grow(double xa, double ya, double za) {
            double _x0 = x0 - xa;
            double _y0 = y0 - ya;
            double _z0 = z0 - za;
            double _x1 = x1 + xa;
            double _y1 = y1 + ya;
            double _z1 = z1 + za;
            return AABB(_x0, _y0, _z0, _x1, _y1, _z1);
        }

        double clipXCollide(AABB c, double xa) {
            if (c.y1 <= y0 || c.y0 >= y1 || c.z1 <= z0 || c.z0 >= z1) {
                return xa;
            }
            double max;
            if (xa > 0.0 && c.x1 <= x0) {
                max = x0 - c.x1 - epsilon;
                if (max < xa) {
                    xa = max;
                }
            }

            if (xa < 0.0 && c.x0 >= x1) {
                max = x1 - c.x0 + epsilon;
                if (max > xa) {
                    xa = max;
                }
            }

            return xa;
        }

        double clipYCollide(AABB c, double ya) {
            if (c.x1 <= x0 || c.x0 >= x1 || c.z1 <= z0 || c.z0 >= z1) {
                return ya;
            }
            double max;
            if (ya > 0.0 && c.y1 <= y0) {
                max = y0 - c.y1 - epsilon;
                if (max < ya) {
                    ya = max;
                }
            }

            if (ya < 0.0 && c.y0 >= y1) {
                max = y1 - c.y0 + epsilon;
                if (max > ya) {
                    ya = max;
                }
            }

            return ya;
        }

        double clipZCollide(AABB c, double za) {
            if (c.x1 <= x0 || c.x0 >= x1 || c.y1 <= y0 || c.y0 >= y1) {
                return za;
            }
            double max;
            if (za > 0.0F && c.z1 <= z0) {
                max = z0 - c.z1 - epsilon;
                if (max < za) {
                    za = max;
                }
            }

            if (za < 0.0F && c.z0 >= z1) {
                max = z1 - c.z0 + epsilon;
                if (max > za) {
                    za = max;
                }
            }

            return za;
        }

        bool intersects(AABB c) {
            if (c.x1 <= x0 || c.x0 >= x1) {
                return false;
            }
            return !(c.y1 <= y0 || c.y0 >= y1 || c.z1 <= z0 || c.z0 >= z1);
        }

        void move(double xa, double ya, double za) {
            x0 += xa;
            y0 += ya;
            z0 += za;
            x1 += xa;
            y1 += ya;
            z1 += za;
        }
    };
}
