#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "real3d/world.h"
#include "real3d/timer.h"
#include "real3d/player.h"
#include "real3d/block.h"
#include "real3d/hit.h"
#include "real3d/client/world_renderer.h"
#include "real3d/client/tesselator.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#include <gl/GLU.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define GLFW_ERROR_EXIT(msg) \
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
using Real3D::Blocks;
using Real3D::World;
using Real3D::WorldRenderer;
using Real3D::Frustum;
using Real3D::HitResult;
using Real3D::Direction;
using Real3D::Tesselator;

GLFWwindow* window;
Timer* timer;
Player* player;
World* world;
WorldRenderer* worldRenderer;
HitResult* hitResult;

GLuint blockAtlas;

int width, height;
double lastX, lastY;
bool grabbing;

GLuint selectBuffer[2000];
GLint viewportBuffer[16];
GLfloat lightBuffer[4];

GLuint crossing;

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
    else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_SPACE) {
            if (timer->lastTime - player->lastSpace <= 0.4 * NS_PER_SECOND) {
                player->flying = !player->flying;
            }
            player->lastSpace = timer->lastTime;
        }

        if (key == GLFW_KEY_1) {
            player->handItems = Blocks::AIR;
        }
        if (key == GLFW_KEY_2) {
            player->handItems = Blocks::GRASS_BLOCK;
        }
        if (key == GLFW_KEY_3) {
            player->handItems = Blocks::STONE;
        }
        if (key == GLFW_KEY_4) {
            player->handItems = Blocks::DIRT;
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
void mouseButtonCb(GLFWwindow*, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (hitResult != nullptr) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                world->setBlock(hitResult->x, hitResult->y, hitResult->z, Blocks::AIR);

            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT
                && player->handItems != Blocks::AIR) {
                int x = hitResult->x,
                    y = hitResult->y,
                    z = hitResult->z;
                switch (hitResult->face) {
                case Direction::NORTH:
                    --z;
                    break;
                case Direction::SOUTH:
                    ++z;
                    break;
                case Direction::WEST:
                    --x;
                    break;
                case Direction::EAST:
                    ++x;
                    break;
                case Direction::UP:
                    ++y;
                    break;
                case Direction::DOWN:
                    --y;
                    break;
                }
                world->setBlock(x, y, z, player->handItems);
            }
        }
    }
}
void resize(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}
void resize(GLFWwindow*, int w, int h) {
    resize(w, h);
}

void moveCameraToPlayer(double delta) {
    glTranslated(0, 0, -0.3);
    glRotated(player->xRot, -1, 0, 0);
    glRotated(player->yRot, 0, 1, 0);
    double x = player->prevX + (player->x - player->prevX) * timer->delta;
    double y = (player->prevY + (player->y - player->prevY) * timer->delta) + player->heightOffset;
    double z = player->prevZ + (player->z - player->prevZ) * timer->delta;
    glTranslated(-x, -y, -z);
}

void setupCamera(double delta) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,
        (GLdouble)width / (GLdouble)height,
        0.05,
        1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(delta);
}

void setupPickCamera(double delta, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    memset(viewportBuffer, 0, 16 * sizeof(GLint));
    glGetIntegerv(GL_VIEWPORT, viewportBuffer);
    gluPickMatrix(x, y, 5.0, 5.0, viewportBuffer);
    gluPerspective(70.0, (GLdouble)width / (GLdouble)height, 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(timer->delta);
}

void setupFog(int layer) {
    if (layer == 0) {
        glDisable(GL_LIGHTING);
    }
    else if (layer == 1) {
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        constexpr float br = 0.6f;
        lightBuffer[0] = br;
        lightBuffer[1] = br;
        lightBuffer[2] = br;
        lightBuffer[3] = 1.0f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightBuffer);
    }
}

void pick(double delta) {
    memset(selectBuffer, 0, 2000 * sizeof(GLuint));
    glSelectBuffer(2000, selectBuffer);
    glRenderMode(GL_SELECT);
    setupPickCamera(delta, width / 2, height / 2);
    worldRenderer->pick(player, Frustum::getFrustum());
    GLint hits = glRenderMode(GL_RENDER);
    long closest = 0L;
    int names[10];
    int hitNameCount = 0;
    int k = 0;
    for (int i = 0; i < hits; ++i) {
        int nameCount = selectBuffer[k];
        ++k;
        long minZ = selectBuffer[k];
        k += 2;
        int j;
        if (minZ >= closest && i != 0) {
            for (j = 0; j < nameCount; ++j) {
                ++k;
            }
        }
        else {
            closest = minZ;
            hitNameCount = nameCount;
            for (j = 0; j < nameCount; ++j) {
                names[j] = selectBuffer[k];
                ++k;
            }
        }
    }
    if (hitNameCount > 0) {
        hitResult = new HitResult(names[0], names[1], names[2], names[3], names[4]);
    }
    else {
        delete hitResult;
        hitResult = nullptr;
    }
}

void drawGui() {
    double screenWidth = width * 240.0 / height;
    double screenHeight = height * 240.0 / height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screenWidth, screenHeight, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -200.0);

    glPushMatrix();
    glTranslated(screenWidth - 16, 16.0, 0.0);
    auto& t = Tesselator::getInstance();
    glScaled(16.0, 16.0, 16.0);
    glRotated(30.0, 1.0, 0.0, 0.0);
    glRotated(45.0, 0.0, -1.0, 0.0);
    glTranslated(-1.5, 0.5, -0.5);
    glScaled(-1.0, -1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);
    glEnable(GL_TEXTURE_2D);
    t.init();
    player->handItems->render(t, world, 0, -2, 0, 0);
    t.flush();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    double wc = screenWidth / 2.0;
    double hc = screenHeight / 2.0;
    glTranslated(wc, hc, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    glCallList(crossing);
    glDisable(GL_BLEND);

}

void render(double delta) {
    pick(delta);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera(delta);
    glEnable(GL_CULL_FACE);
    worldRenderer->updateDirtyChunks(player);
    setupFog(0);
    worldRenderer->render(player, 0, blockAtlas);
    setupFog(1);
    worldRenderer->render(player, 1, blockAtlas);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    if (hitResult != nullptr) {
        glDisable(GL_ALPHA_TEST);
        worldRenderer->renderHit(hitResult);
        glEnable(GL_ALPHA_TEST);
    }

    drawGui();

    glfwSwapBuffers(window);
}

void tick() {
    world->randomTick(player);
    player->tick();
}

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    glfwSetErrorCallback(errorCb);
    if (!glfwInit()) {
        GLFW_ERROR_EXIT("Can't init GLFW!");
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Real3D for Windows", nullptr, nullptr);
    if (window == nullptr) {
        GLFW_ERROR_EXIT("Can't create window!");
    }

    glfwSetKeyCallback(window, keyCb);
    glfwSetCursorPosCallback(window, cursorPosCb);
    glfwSetMouseButtonCallback(window, mouseButtonCb);
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

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_PROJECTION);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    resize(WIDTH, HEIGHT);

    glGenTextures(1, &blockAtlas);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    {
        int w, h, c;
        stbi_uc* pixels = stbi_load("res/block-atlas.png", &w, &h, &c, STBI_rgb_alpha);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    }

    Blocks::init();

    world = new World();
    worldRenderer = new WorldRenderer(world);
    player = new Player(world);

    timer = new Timer(20.0);
    timer->advanceTime();

    glfwShowWindow(window);

    double lastTime = glfwGetTime() * 1000;
    int frames = 0;
    player->lastSpace = timer->passedTime;

    auto& t = Tesselator::getInstance();
    crossing = glGenLists(1);
    glNewList(crossing, GL_COMPILE);
    t.init();
    t.color(1, 1, 1)
        .vertex(1, -4, 0)
        .vertex(0, -4, 0)
        .vertex(0, 5, 0)
        .vertex(1, 5, 0)
        .vertex(5, 0, 0)
        .vertex(1, 0, 0)
        .vertex(1, 1, 0)
        .vertex(5, 1, 0)
        .vertex(0, 0, 0)
        .vertex(-4, 0, 0)
        .vertex(-4, 1, 0)
        .vertex(0, 1, 0);
    t.flush();
    glEndList();

    while (!glfwWindowShouldClose(window)) {
        timer->advanceTime();
        for (int i = 0; i < timer->ticks; ++i) {
            tick();
        }
        render(timer->delta);
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
