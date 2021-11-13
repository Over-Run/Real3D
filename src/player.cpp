#include <ctime>
#include "gmath.h"
#include "real3d/world.h"
#include "real3d/player.h"
#include "GLFW/glfw3.h"

using std::vector;
using Real3D::Player;

Player::Player(World* _world) :
    world(_world),
    handItems(Blocks::GRASS_BLOCK) {
    resetPos();
}
void Player::resetPos() {
    srand(time(nullptr));
    double x = (rand() % 10) / 10.0 * world->width;
    double y = world->height + 10;
    double z = (rand() % 10) / 10.0 * world->depth;
    setPos(x, y, z);
}
void Player::setSize(double w, double h) {
    bbWidth = w;
    bbHeight = h;
}
void Player::setPos(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
    double w = bbWidth / 2.0;
    double h = bbHeight / 2.0;
    bb = new AABB(_x - w, _y - h, _z - w, _x + w, _y + h, _z + w);
}
void Player::tick() {
    prevX = x;
    prevY = y;
    prevZ = z;
    auto wnd = glfwGetCurrentContext();
    double xa = 0, za = 0;
    if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) {
        --za;
    }
    if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) {
        ++za;
    }
    if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) {
        --xa;
    }
    if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) {
        ++xa;
    }
    if (glfwGetKey(wnd, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (flying || onGround) {
            yd = 0.5;
        }
    }
    double speed = 0.2;
    if (!flying) {
        if (onGround) {
            speed = 0.1;
        }
        else {
            speed = 0.02;
        }
    }
    if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (!flying)
            heightOffset = 1.32;
        bbHeight = 1.5;
        speed -= 0.01;
        if (flying) {
            yd = -0.5;
        }
    }
    else {
        heightOffset = 1.62;
        bbHeight = 1.8;
    }
    moveRelative(xa, za, speed);
    if (!flying)
        yd -= 0.08;
    move(xd, yd, zd);
    xd *= 0.91;
    yd *= 0.98;
    zd *= 0.91;
    if (onGround || flying) {
        xd *= 0.7;
        zd *= 0.7;
    }
    if (flying) {
        yd *= 0.5;
    }
}
void Player::move(double xa, double ya, double za) {
    double xaOrg = xa;
    double yaOrg = ya;
    double zaOrg = za;
    vector<AABB*> cubes = world->getCubes(bb->expand(xa, ya, za));
    for (AABB* aabb : cubes) {
        ya = aabb->clipYCollide(bb, ya);
    }
    bb->move(0.0, ya, 0.0);
    for (AABB* aabb : cubes) {
        xa = aabb->clipXCollide(bb, xa);
    }
    bb->move(xa, 0.0, 0.0);
    for (AABB* aabb : cubes) {
        za = aabb->clipZCollide(bb, za);
    }
    bb->move(0.0, 0.0, za);
    for (auto& b : cubes) {
        delete b;
    }
    onGround = yaOrg != ya && yaOrg < 0.0;
    if (onGround
        && glfwGetKey(
            glfwGetCurrentContext(),
            GLFW_KEY_LEFT_SHIFT
        ) == GLFW_PRESS) {
        flying = false;
    }
    if (xaOrg != xa) {
        xd = 0.0;
    }

    if (yaOrg != ya) {
        yd = 0.0;
    }

    if (zaOrg != za) {
        zd = 0.0;
    }
    x = (bb->x0 + bb->x1) / 2.0;
    y = bb->y0;
    z = (bb->z0 + bb->z1) / 2.0;
}
void Player::moveRelative(double xa, double za, double speed) {
    float dist = xa * xa + za * za;
    if (dist >= 0.01) {
        dist = speed / sqrt(dist);
        xa *= dist;
        za *= dist;
        double sin = ::sin(yRot * PI / 180);
        double cos = ::cos(yRot * PI / 180);
        xd += xa * cos - za * sin;
        zd += za * cos + xa * sin;
    }
}
void Player::turn(double xOffset, double yOffset) {
    xRot += yOffset;
    yRot += xOffset;
    if (xRot > 90) {
        xRot = 90;
    }
    else if (xRot < -90) {
        xRot = -90;
    }
    if (yRot >= 360) {
        yRot = 0;
    }
    else if (yRot <= 0) {
        yRot = 360;
    }
}
