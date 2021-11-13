#pragma once
#include "client/frustum.h"

namespace Real3D {
    enum class Direction {
        NORTH,
        SOUTH,
        WEST,
        EAST,
        UP,
        DOWN
    };

    static Direction convertFaceToDir(int face) {
        switch (face) {
        case Frustum::RIGHT:
            return Direction::EAST;
        case Frustum::LEFT:
            return Direction::WEST;
        case Frustum::BOTTOM:
            return Direction::DOWN;
        case Frustum::TOP:
            return Direction::UP;
        case Frustum::FRONT:
            return Direction::SOUTH;
        }
        return Direction::NORTH;
    }

    static int convertDirToFace(Direction dir) {
        switch (dir) {
        case Direction::EAST:
            return Frustum::RIGHT;
        case Direction::WEST:
            return Frustum::LEFT;
        case Direction::DOWN:
            return Frustum::BOTTOM;
        case Direction::UP:
            return Frustum::TOP;
        case Direction::SOUTH:
            return Frustum::FRONT;
        }
        return Frustum::BACK;
    }
}
