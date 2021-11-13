#include "real3d/client/tesselator.h"
#include <memory>

using Real3D::Tesselator;

Tesselator::Tesselator() :
    len(3) {}
Tesselator& Tesselator::getInstance() {
    static Tesselator t;
    return t;
}
void Tesselator::flush() {
    if (hasTexture && hasColor) {
        glInterleavedArrays(GL_T2F_C3F_V3F, 0, array);
    }
    else if (hasTexture) {
        glInterleavedArrays(GL_T2F_V3F, 0, array);
    }
    else if (hasColor) {
        glInterleavedArrays(GL_C3F_V3F, 0, array);
    }
    else {
        glInterleavedArrays(GL_V3F, 0, array);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    if (hasTexture) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (hasColor) {
        glEnableClientState(GL_COLOR_ARRAY);
    }

    glDrawArrays(GL_QUADS, 0, vertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (hasTexture) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (hasColor) {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    clear();
}
void Tesselator::clear() {
    vertices = 0;
    memset(array, 0, MAX_FLOATS * sizeof(GLfloat));
    p = 0;
}
void Tesselator::init() {
    clear();
    hasColor = false;
    hasTexture = false;
}
Tesselator& Tesselator::tex(GLfloat _u, GLfloat _v) {
    if (!hasTexture) {
        len += 2;
    }
    hasTexture = true;
    u = _u;
    v = _v;
    return *this;
}
Tesselator& Tesselator::color(GLfloat _r, GLfloat _g, GLfloat _b) {
    if (!hasColor) {
        len += 3;
    }
    hasColor = true;
    r = _r;
    g = _g;
    b = _b;
    return *this;
}
Tesselator& Tesselator::vertexUV(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _u, GLfloat _v) {
    return tex(_u, _v).vertex(_x, _y, _z);
}
Tesselator& Tesselator::vertex(GLfloat _x, GLfloat _y, GLfloat _z) {
    if (hasTexture) {
        array[p++] = u;
        array[p++] = v;
    }

    if (hasColor) {
        array[p++] = r;
        array[p++] = g;
        array[p++] = b;
    }

    array[p++] = _x;
    array[p++] = _y;
    array[p++] = _z;
    ++vertices;
    if (vertices % 4 == 0 && p >= MAX_FLOATS - len * 4) {
        flush();
    }

    return *this;
}
