#pragma once
#include "glad/gl.h"

namespace Real3D {
    class Tesselator {
    private:
        static constexpr int MAX_MEMORY_USE = 4194304;
        static constexpr int MAX_FLOATS = MAX_MEMORY_USE / 8;
        GLfloat array[MAX_FLOATS];
        int vertices;
        GLfloat u;
        GLfloat v;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        bool hasColor;
        bool hasTexture;
        int len;
        int p;
        Tesselator();
        void clear();
    public:
        Tesselator(const Tesselator&) = delete;
        Tesselator operator=(const Tesselator&) = delete;
        static Tesselator& getInstance();
        void flush();
        void init();
        Tesselator& tex(GLfloat _u, GLfloat _v);
        Tesselator& color(GLfloat _r, GLfloat _g, GLfloat _b);
        Tesselator& vertexUV(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _u, GLfloat _v);
        Tesselator& vertex(GLfloat _x, GLfloat _y, GLfloat _z);
    };
}
