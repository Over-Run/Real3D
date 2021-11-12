#include "real3d/world_renderer.h"
#include "real3d/hit.h"
#include "real3d/world.h"

using Real3D::WorldRenderer;

extern "C" double glfwGetTime(void);

WorldRenderer::WorldRenderer(World* w) :
    world(w) {}
void WorldRenderer::pick(Player* player, Frustum& frustum) {
    double r = 5;
    AABB box = player->bb->grow(r, r, r);
    int x0 = (int)box.x0;
    int x1 = (int)(box.x1 + 1.0);
    int y0 = (int)box.y0;
    int y1 = (int)(box.y1 + 1.0);
    int z0 = (int)box.z0;
    int z1 = (int)(box.z1 + 1.0);
    glInitNames();
    glPushName(0);
    glPushName(0);

    for (int x = x0; x < x1; ++x) {
        glLoadName(x);
        glPushName(0);

        for (int y = y0; y < y1; ++y) {
            glLoadName(y);
            glPushName(0);

            for (int z = z0; z < z1; ++z) {
                Block* block = world->getBlock(x, y, z);
                auto box = block->getOutline();
                if (box != nullptr) {
                    auto cpy = new AABB(*box);
                    cpy->move(x, y, z);
                    if (frustum.isVisible(cpy)) {
                        glLoadName(z);
                        glPushName(0);

                        for (int i = 0; i < 6; ++i) {
                            glLoadName(i);
                            glBegin(GL_QUADS);
                            block->pickFace(x, y, z, convertFaceToDir(i));
                            glEnd();
                        }

                        glPopName();
                    }
                    delete cpy;
                }
            }

            glPopName();
        }

        glPopName();
    }

    glPopName();
    glPopName();
}
void WorldRenderer::render(GLuint blockAtlas) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);

    glBegin(GL_QUADS);
    for (int x = 0; x < world->width; ++x) {
        for (int y = 0; y < world->height; ++y) {
            for (int z = 0; z < world->depth; ++z) {
                world->getBlock(x, y, z)->render(world, x, y, z);
            }
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
void WorldRenderer::renderHit(HitResult* h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4d(1.0, 1.0, 1.0, (sin(glfwGetTime() * 10.0) * 0.2 + 0.4) * 0.5);
    glBegin(GL_QUADS);
    Blocks::STONE->pickFace(h->x, h->y, h->z, h->face);
    glEnd();
    glDisable(GL_BLEND);
}
