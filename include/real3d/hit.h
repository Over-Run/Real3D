#pragma once
#include "enums.h"

namespace Real3D {
    class HitResult {
    public:
        int x, y, z;
        Direction face;

        HitResult(int, int _x, int _y, int _z, int _face) :
            x(_x),
            y(_y),
            z(_z),
            face(convertFaceToDir(_face)) {}
    };
}
