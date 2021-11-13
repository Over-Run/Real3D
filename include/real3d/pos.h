#pragma once

namespace Real3D {
    class BlockPos {
    public:
        friend struct std::hash<BlockPos>;
        int x, y, z;

        BlockPos(int _x = 0,
            int _y = 0,
            int _z = 0) :
            x(_x), y(_y), z(_z) {}

        bool operator==(const BlockPos& p) const {
            return x == p.x && y == p.y && z == p.z;
        }

        bool operator!=(const BlockPos& p) const {
            return x != p.x || y != p.y || z != p.z;
        }

        bool operator<(const BlockPos& p) const {
            return z < p.z || y < p.y || x < p.x;
        }

        bool operator()(const BlockPos& l, const BlockPos& r) const {
            return l == r;
        }
    };
}

template<>
struct std::hash<Real3D::BlockPos> {
    int operator()(const Real3D::BlockPos& p) const {
        return std::hash<int>{}(p.x * 31 * 31 + p.y * 31 + p.z);
    }
};
