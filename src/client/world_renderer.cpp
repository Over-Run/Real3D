#include "real3d/client/world_renderer.h"
#include "real3d/client/chunk.h"
#include "real3d/hit.h"
#include "real3d/world.h"
#include "GLFW/glfw3.h"

using Real3D::WorldRenderer;

WorldRenderer::WorldRenderer(World* w) :
    world(w),
    xChunks(World::width / CHUNK_SIZE),
    yChunks(World::height / CHUNK_SIZE),
    zChunks(World::depth / CHUNK_SIZE) {
    w->addListener(this);
}
WorldRenderer::~WorldRenderer() {
    for (auto& p : chunks) {
        delete p.second;
    }
    chunks.clear();
}
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
    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                auto p = BlockPos(x, y, z);
                if (chunks.count(p) < 1) {
                    int x0 = x * CHUNK_SIZE;
                    int y0 = y * CHUNK_SIZE;
                    int z0 = z * CHUNK_SIZE;
                    int x1 = (x + 1) * CHUNK_SIZE;
                    int y1 = (y + 1) * CHUNK_SIZE;
                    int z1 = (z + 1) * CHUNK_SIZE;
                    if (x1 > World::width) {
                        x1 = World::width;
                    }
                    if (y1 > World::height) {
                        y1 = World::height;
                    }
                    if (z1 > World::depth) {
                        z1 = World::depth;
                    }
                    chunks[p] = new Chunk(world, x0, y0, z0, x1, y1, z1);
                    chunks[p]->rebuild();
                }
                if (chunks[p]->isDirty()) {
                    chunks[p]->rebuild();
                }
            }
        }
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);

    auto& frustum = Frustum::getFrustum();

    glBegin(GL_QUADS);
    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                auto& c = chunks[BlockPos(x, y, z)];
                if (frustum.isVisible(c->aabb)) {
                    c->render();
                }
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
void WorldRenderer::markDirty(int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= CHUNK_SIZE;
    x1 /= CHUNK_SIZE;
    y0 /= CHUNK_SIZE;
    y1 /= CHUNK_SIZE;
    z0 /= CHUNK_SIZE;
    z1 /= CHUNK_SIZE;
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (z0 < 0) {
        z0 = 0;
    }
    if (x1 >= xChunks) {
        x1 = xChunks - 1;
    }
    if (y1 >= yChunks) {
        y1 = yChunks - 1;
    }
    if (z1 >= zChunks) {
        z1 = zChunks - 1;
    }
    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            for (int z = z0; z <= z1; ++z) {
                chunks[BlockPos(x, y, z)]->markDirty();
            }
        }
    }
}
void WorldRenderer::blockChanged(int x, int y, int z) {
    markDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}
void WorldRenderer::lightColumnChanged(int x, int z, int y0, int y1) {}
void WorldRenderer::allChanged() {}
