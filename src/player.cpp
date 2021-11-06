#include "gmath.h"
#include "real3d/player.h"
#include "GLFW/glfw3.h"

using Real3D::Player;

Player::Player() {
    double w = bbWidth / 2.0;
    double h = bbHeight / 2.0;
    bb = new AABB(x - w, y - h, z - w, x + w, y + h, z + w);
}
void Player::tick() {
    prevX = x;
    prevY = y;
    prevZ = z;
    auto wnd = glfwGetCurrentContext();
    int xa = 0, ya = 0, za = 0;
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
        ++ya;
    }
    if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        --ya;
    }
    moveRelative(xa, za, onGround ? 0.1 : 0.02);
    y += 0.02 * ya;
    /*yd -= 0.08;
    move(xd, yd, zd);
    xd *= 0.91;
    yd *= 0.98;
    zd *= 0.91;
    if (onGround) {
        xd *= 0.7;
        zd *= 0.7;
    }*/
}
void Player::move(double xa, double ya, double za) {
    double xaOrg = xa;
    double yaOrg = ya;
    double zaOrg = za;
    bb->move(0.0, ya, 0.0);
    bb->move(xa, 0.0, 0.0);
    bb->move(0.0, 0.0, za);
    onGround = yaOrg != ya && yaOrg < 0.0;
    if (xaOrg != xa) {
        xd = 0.0;
    }

    if (yaOrg != ya) {
        yd = 0.0;
    }

    if (zaOrg != za) {
        zd = 0.0;
    }
    x = (bb->x0 + bb->x1) / 2.0F;
    y = bb->y0 + heightOffset;
    z = (bb->z0 + bb->z1) / 2.0F;
}
void Player::moveRelative(double xa, double za, double speed) {
    float dist = xa * xa + za * za;
    if (dist >= 0.01) {
        dist = speed / sqrt(dist);;
        xa *= dist;
        za *= dist;
        double sin = ::sin(yRot * PI / 180);
        double cos = ::cos(yRot * PI / 180);
        //here
        x += xa * cos - za * sin;
        z += za * cos + xa * sin;
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
