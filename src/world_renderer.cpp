#include <algorithm>
#include "real3d/world_renderer.h"
#include "real3d/world.h"
#include "real3d/chunk.h"
#include "real3d/hit.h"
#include "GLFW/glfw3.h"

using std::vector;
using std::sort;
using Real3D::Chunk;
using Real3D::WorldRenderer;

WorldRenderer::WorldRenderer(World* _world) : world(_world) {
    _world->addListener(this);
    xChunks = world->width / CHUNK_SIZE;
    yChunks = world->depth / CHUNK_SIZE;
    zChunks = world->height / CHUNK_SIZE;
    chunksSize = xChunks * yChunks * zChunks;
    chunks = new Chunk*[chunksSize];
    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                int x0 = x * CHUNK_SIZE;
                int y0 = y * CHUNK_SIZE;
                int z0 = z * CHUNK_SIZE;
                int x1 = (x + 1) * CHUNK_SIZE;
                int y1 = (y + 1) * CHUNK_SIZE;
                int z1 = (z + 1) * CHUNK_SIZE;
                if (x1 > world->width) {
                    x1 = world->width;
                }
                if (y1 > world->height) {
                    y1 = world->height;
                }
                if (z1 > world->depth) {
                    z1 = world->depth;
                }
                chunks[(x + y * xChunks) * zChunks + z] = new Chunk(world, x0, y0, z0, x1, y1, z1);
            }
        }
    }
}
WorldRenderer::~WorldRenderer() {
    for (size_t i = 0; i < chunksSize; ++i) {
        delete chunks[i];
    }
    delete[] chunks;
}
vector<Chunk*>* WorldRenderer::getAllDirtyChunks() {
    vector<Chunk*>* dirty = nullptr;
    for (size_t i = 0; i < chunksSize; ++i) {
        Chunk* chunk = chunks[i];
        if (chunk->isDirty()) {
            if (dirty == nullptr) {
                dirty = new vector<Chunk*>();
            }
            dirty->push_back(chunk);
        }
    }
    return dirty;
}
void WorldRenderer::render(GLuint blockAtlas) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, blockAtlas);
    Frustum frustum = Frustum::getFrustum();
    glBegin(GL_QUADS);
    for (int i = 0; i < chunksSize; ++i) {
        auto chunk = chunks[i];
        if (frustum.isVisible(chunk->aabb)) {
            chunk->render();
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void WorldRenderer::updateDirtyChunks(Player* player) {
    auto dirty = getAllDirtyChunks();
    if (dirty != nullptr) {
        sort(dirty->begin(), dirty->end());
        for (int i = 0;
            i < WorldRenderer::MAX_REBUILDS_PER_FRAME && i < dirty->size();
            ++i) {
            (*dirty)[i]->rebuild();
        }
    }
    delete dirty;
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
void WorldRenderer::renderHit(HitResult* h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4d(1.0, 1.0, 1.0, (sin(glfwGetTime() * 10.0) * 0.2 + 0.4) * 0.5);
    glBegin(GL_QUADS);
    Blocks::STONE->pickFace(h->x,
        h->y,
        h->z,
        h->face);
    glEnd();
    glDisable(GL_BLEND);
}
void WorldRenderer::setDirty(int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= 16;
    x1 /= 16;
    y0 /= 16;
    y1 /= 16;
    z0 /= 16;
    z1 /= 16;
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
                chunks[(x + y * xChunks) * zChunks + z]->setDirty();
            }
        }
    }
}
void WorldRenderer::blockChanged(int x, int y, int z) {
    setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}
void WorldRenderer::lightColumnChanged(int x, int z, int y0, int y1) {
    setDirty(x - 1, y0 - 1, z - 1, x + 1, y1 + 1, z + 1);
}
void WorldRenderer::allChanged() {
    setDirty(0, 0, 0, world->width, world->height, world->depth);
}
