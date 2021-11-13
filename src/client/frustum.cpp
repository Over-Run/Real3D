#include <cmath>
#include <memory>
#include "real3d/client/frustum.h"
#include "real3d/aabb.h"
#include "glad/gl.h"

using Real3D::Frustum;

Frustum& Frustum::getFrustum() {
    static Frustum f;
    f.calculateFrustum();
    return f;
}
void Frustum::normalizePlane(float frustum[6][4], int side) {
    float magnitude = sqrt(frustum[side][A] * frustum[side][A] + frustum[side][B] * frustum[side][B] + frustum[side][C] * frustum[side][C]);
    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude;
}
void Frustum::calculateFrustum() {
    memset(_proj, 0, 16 * sizeof(float));
    memset(_modl, 0, 16 * sizeof(float));
    memset(_clip , 0, 16 * sizeof(float));
    glGetFloatv(GL_PROJECTION_MATRIX, _proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, _modl);
    for (int i = 0; i < 16; ++i) {
        proj[i] = _proj[i];
        modl[i] = _modl[i];
    }
    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];
    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];
    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];
    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];
    m_Frustum[RIGHT][A] = clip[3] - clip[0];
    m_Frustum[RIGHT][B] = clip[7] - clip[4];
    m_Frustum[RIGHT][C] = clip[11] - clip[8];
    m_Frustum[RIGHT][D] = clip[15] - clip[12];
    normalizePlane(m_Frustum, RIGHT);
    m_Frustum[LEFT][A] = clip[3] + clip[0];
    m_Frustum[LEFT][B] = clip[7] + clip[4];
    m_Frustum[LEFT][C] = clip[11] + clip[8];
    m_Frustum[LEFT][D] = clip[15] + clip[12];
    normalizePlane(m_Frustum, LEFT);
    m_Frustum[BOTTOM][A] = clip[3] + clip[1];
    m_Frustum[BOTTOM][B] = clip[7] + clip[5];
    m_Frustum[BOTTOM][C] = clip[11] + clip[9];
    m_Frustum[BOTTOM][D] = clip[15] + clip[13];
    normalizePlane(m_Frustum, BOTTOM);
    m_Frustum[TOP][A] = clip[3] - clip[1];
    m_Frustum[TOP][B] = clip[7] - clip[5];
    m_Frustum[TOP][C] = clip[11] - clip[9];
    m_Frustum[TOP][D] = clip[15] - clip[13];
    normalizePlane(m_Frustum, TOP);
    m_Frustum[BACK][A] = clip[3] - clip[2];
    m_Frustum[BACK][B] = clip[7] - clip[6];
    m_Frustum[BACK][C] = clip[11] - clip[10];
    m_Frustum[BACK][D] = clip[15] - clip[14];
    normalizePlane(m_Frustum, BACK);
    m_Frustum[FRONT][A] = clip[3] + clip[2];
    m_Frustum[FRONT][B] = clip[7] + clip[6];
    m_Frustum[FRONT][C] = clip[11] + clip[10];
    m_Frustum[FRONT][D] = clip[15] + clip[14];
    normalizePlane(m_Frustum, FRONT);
}
bool Frustum::pointInFrustum(float x, float y, float z) {
    for (int i = 0; i < 6; ++i) {
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0.0f) {
            return false;
        }
    }
    return true;
}
bool Frustum::sphereInFrustum(float x, float y, float z, float radius) {
    for (int i = 0; i < 6; ++i) {
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius) {
            return false;
        }
    }
    return true;
}
bool Frustum::cubeFullyInFrustum(float x1, float y1, float z1, float x2, float y2, float z2) {
    for (int i = 0; i < 6; ++i) {
        if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            && m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f)
            ) {
            return false;
        }

    }
    return true;
}
bool Frustum::cubeInFrustum(float x1, float y1, float z1, float x2, float y2, float z2) {
    for (int i = 0; i < 6; ++i) {
        if (!(m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z1 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x1 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x2 + m_Frustum[i][B] * y1 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x1 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f
            || m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0.0f)
            ) {
            return false;
        }
    }
    return true;
}
bool Frustum::isVisible(AABB* aabb) {
    return cubeInFrustum(aabb->x0, aabb->y0, aabb->z0, aabb->x1, aabb->y1, aabb->z1);
}
