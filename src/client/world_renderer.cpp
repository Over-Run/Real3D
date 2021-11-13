#include "real3d/client/chunk.h"
#include "real3d/hit.h"
#include "real3d/world.h"
#include "GLFW/glfw3.h"
#include <algorithm>

using Real3D::WorldRenderer;
using Real3D::Chunk;
using std::vector;
using std::sort;

WorldRenderer::WorldRenderer(World* w) :
    world(w),
    xChunks(World::width / CHUNK_SIZE),
    yChunks(World::height / CHUNK_SIZE),
    zChunks(World::depth / CHUNK_SIZE) {
    w->addListener(this);
    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                auto p = BlockPos(x, y, z);
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
            }
        }
    }
}
WorldRenderer::~WorldRenderer() {
    for (auto& p : chunks) {
        delete p.second;
    }
    chunks.clear();
}
vector<Chunk*>* WorldRenderer::getAllDirtyChunks() {
    vector<Chunk*>* dirty = nullptr;
    for (auto& p : chunks) {
        auto& chunk = p.second;
        if (chunk->isDirty()) {
            if (dirty == nullptr) {
                dirty = new vector<Chunk*>();
            }
            dirty->push_back(chunk);
        }
    }
    return dirty;
}
void WorldRenderer::updateDirtyChunks(Player* player) {
    auto dirty = getAllDirtyChunks();
    if (dirty != nullptr) {
        auto& frustum = Frustum::getFrustum();
        sort(dirty->begin(),
            dirty->end(),
            [&frustum, player](Chunk* c0, Chunk* c1) {
                double now = glfwGetTime() * 1000;
                bool i0 = frustum.isVisible(c0->aabb);
                bool i1 = frustum.isVisible(c1->aabb);
                if (i0 && !i1) return true;
                if (i1 && !i0) return false;
                auto t0 = (now - c0->dirtiedTime) / 2000.0;
                auto t1 = (now - c1->dirtiedTime) / 2000.0;
                if (t0 < t1) return true;
                if (t0 > t1) return false;
                return c0->distanceToSqr(player) < c1->distanceToSqr(player);
            }
        );
        for (int i = 0; i < MAX_REBUILDS_PER_FRAME && i < dirty->size(); ++i) {
            (*dirty)[i]->rebuild();
        }
    }
    delete dirty;
}
void WorldRenderer::pick(Player* player, Frustum& frustum) {
    auto& t = Tesselator::getInstance();
    double r = 5.0;
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
                            t.init();
                            block->pickFace(t, x, y, z, convertFaceToDir(i));
                            t.flush();
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
void WorldRenderer::render(Player* player, GLuint blockAtlas) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);
    auto& frustum = Frustum::getFrustum();
    // Rendering radius
    constexpr int r = 8;
    int x0 = player->x / CHUNK_SIZE - r;
    int y0 = player->y / CHUNK_SIZE - r;
    int z0 = player->z / CHUNK_SIZE - r;
    int x1 = player->x / CHUNK_SIZE + r;
    int y1 = player->y / CHUNK_SIZE + r;
    int z1 = player->z / CHUNK_SIZE + r;
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
    for (int x = x0; x < x1; ++x) {
        for (int y = y0; y < y1; ++y) {
            for (int z = z0; z < z1; ++z) {
                auto& c = chunks[BlockPos(x, y, z)];
                if (frustum.isVisible(c->aabb)) {
                    c->render();
                }
            }
        }
    }
    /*for (auto& p : chunks) {
        auto& c = p.second;
        if (frustum.isVisible(c->aabb)) {
            c->render();
        }
    }*/
    glDisable(GL_TEXTURE_2D);
}
void WorldRenderer::renderHit(HitResult* h) {
    auto& t = Tesselator::getInstance();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4d(1.0, 1.0, 1.0, (sin(glfwGetTime() * 10.0) * 0.2 + 0.4) * 0.5);
    t.init();
    Blocks::STONE->pickFace(t, h->x, h->y, h->z, h->face);
    t.flush();
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
