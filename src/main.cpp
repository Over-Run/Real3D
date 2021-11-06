#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include <gl/GLU.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "real3d/player.h"
#include "real3d/timer.h"

#define ERROR_EXIT(msg) \
const char* desc; \
int code = glfwGetError(&desc); \
cout << msg" Error " << code << ": " << desc << endl; \
return code

constexpr int WIDTH = 854;
constexpr int HEIGHT = 480;
constexpr auto SENSITIVITY = 0.05;

using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::stringstream;
using std::string;
using Real3D::Timer;
using Real3D::Player;

GLFWwindow* window;
Timer* timer;
Player* player;

GLuint blockAtlas;

double lastX, lastY;
boolean grabbing;

void errorCb(int code, const char* desc) {
    cout << "Error " << code << ": " << desc << endl;
}

void keyCb(GLFWwindow*, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (key == GLFW_KEY_GRAVE_ACCENT) {
            grabbing = !grabbing;
            glfwSetInputMode(window,
                GLFW_CURSOR,
                grabbing ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
}

void cursorPosCb(GLFWwindow*, double xpos, double ypos) {
    if (grabbing) {
        double xOffset = xpos - lastX;
        double yOffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        xOffset *= SENSITIVITY;
        yOffset *= SENSITIVITY;
        player->turn(xOffset, yOffset);
    }
    lastX = xpos;
    lastY = ypos;
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,
        (GLdouble)w / (GLdouble)h,
        0.05,
        1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void resize(GLFWwindow*, int w, int h) {
    resize(w, h);
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    glfwSetErrorCallback(errorCb);
    if (!glfwInit()) {
        ERROR_EXIT("Can't init GLFW!");
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Real3D for Windows", nullptr, nullptr);
    if (window == nullptr) {
        ERROR_EXIT("Can't create window!");
    }

    glfwSetKeyCallback(window, keyCb);
    glfwSetCursorPosCallback(window, cursorPosCb);
    glfwSetFramebufferSizeCallback(window, resize);

    auto vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowPos(window,
            (vidmode->width - w) / 2,
            (vidmode->height - h) / 2);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    //glfwSwapInterval(1);

    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    resize(WIDTH, HEIGHT);

    glGenTextures(1, &blockAtlas);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int w, h, c;
    stbi_uc* pixels = stbi_load("res/block-atlas.png", &w, &h, &c, STBI_rgb_alpha);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);

    player = new Player();

    timer = new Timer(20.0);
    timer->advanceTime();

    glfwShowWindow(window);

    double lastTime = glfwGetTime() * 1000;
    int frames = 0;
    while (!glfwWindowShouldClose(window)) {
        timer->advanceTime();
        for (int i = 0; i < timer->ticks; ++i) {
            player->tick();
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
        glTranslated(0, 0, -0.3);
        glRotated(player->xRot, -1, 0, 0);
        glRotated(player->yRot, 0, 1, 0);
        double x = player->prevX + (player->x - player->prevX) * timer->delta;
        double y = player->prevY + (player->y - player->prevY) * timer->delta;
        double z = player->prevZ + (player->z - player->prevZ) * timer->delta;
        glTranslated(-x, -y, -z);
        glBindTexture(GL_TEXTURE_2D, blockAtlas);
        glColor3d(1, 1, 1);

        glBegin(GL_QUADS);
        auto u0 = 0.0;
        auto u1 = 16.0 / 256;
        auto v0 = 0.0;
        auto v1 = 16.0 / 256;
        // North
        glTexCoord2d(u0, v0); glVertex3d(1, 0, 0);
        glTexCoord2d(u1, v0); glVertex3d(0, 0, 0);
        glTexCoord2d(u1, v1); glVertex3d(0, 1, 0);
        glTexCoord2d(u0, v1); glVertex3d(1, 1, 0);
        // South
        glTexCoord2d(u0, v0); glVertex3d(0, 0, 1);
        glTexCoord2d(u1, v0); glVertex3d(1, 0, 1);
        glTexCoord2d(u1, v1); glVertex3d(1, 1, 1);
        glTexCoord2d(u0, v1); glVertex3d(0, 1, 1);
        // West
        glTexCoord2d(u0, v0); glVertex3d(0, 0, 0);
        glTexCoord2d(u1, v0); glVertex3d(0, 0, 1);
        glTexCoord2d(u1, v1); glVertex3d(0, 1, 1);
        glTexCoord2d(u0, v1); glVertex3d(0, 1, 0);
        // East
        glTexCoord2d(u0, v0); glVertex3d(1, 0, 1);
        glTexCoord2d(u1, v0); glVertex3d(1, 0, 0);
        glTexCoord2d(u1, v1); glVertex3d(1, 1, 0);
        glTexCoord2d(u0, v1); glVertex3d(1, 1, 1);
        // Up
        glTexCoord2d(u0, v0); glVertex3d(0, 1, 1);
        glTexCoord2d(u1, v0); glVertex3d(1, 1, 1);
        glTexCoord2d(u1, v1); glVertex3d(1, 1, 0);
        glTexCoord2d(u0, v1); glVertex3d(0, 1, 0);
        // Down
        glTexCoord2d(u0, v0); glVertex3d(0, 0, 0);
        glTexCoord2d(u1, v0); glVertex3d(1, 0, 0);
        glTexCoord2d(u1, v1); glVertex3d(1, 0, 1);
        glTexCoord2d(u0, v1); glVertex3d(0, 0, 1);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
        ++frames;
        while (glfwGetTime() * 1000 >= lastTime + 1000) {
            lastTime += 1000;
            frames = 0;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    glfwSetErrorCallback(nullptr);
    return 0;
}
