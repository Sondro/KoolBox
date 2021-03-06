#include "GameScene.h"
#include "Game.h"
#include "Programs.h"
#include <Kore/Graphics2/Graphics.h>

using namespace Kore;
using namespace KoolBox;

namespace
{
    typedef struct Bunny
    {
        float x, xv, y, yv;
    } Bunny;

    Kore::Graphics2::Graphics2 *G2;

    Bunny bunnies[50000] = {0};
    int bunny_c = 0;
    Texture *btex = nullptr;
    TextureRegion *breg = nullptr;
    float minX = 0;
    float maxX = 0;
    float minY = 0;
    float maxY = 0;
    float fpsTime = 0;
    int   fpsCount = 0;

    int randomInt(int min, int max)
    {
        return Random::get(min, max);
    }

    void initBunny(Bunny *bunny)
    {
        bunny->x = minX + randomInt((int) minX, (int) maxX);
        bunny->y = minY + randomInt((int) minY, (int) maxY);

        do { bunny->xv = 100 - randomInt(0, 200); } while (bunny->xv < 10 && bunny->xv > -10);
        do { bunny->yv = 100 - randomInt(0, 200); } while (bunny->yv < 10 && bunny->yv > -10);
    }

    void moreBunnies(int more)
    {
        for (int i = 0; i < more; i++)
        {
            initBunny(&bunnies[bunny_c++]);
        }
    }

    void lessBunnies()
    {
        bunny_c -= 1000;
        if (bunny_c <= 1000) {
            bunny_c = 1000;
        }
    }

    void updateBunnies(float dt)
    {
        Bunny *bunny = bunnies;
        for (int i = 0; i < bunny_c; i++, bunny++)
        {
            bunny->x += dt * bunny->xv;
            if (bunny->x < minX) { bunny->x = minX; bunny->xv = -bunny->xv; }
            if (bunny->x > maxX) { bunny->x = maxX; bunny->xv = -bunny->xv; }

            bunny->y += dt * bunny->yv;
            if (bunny->y < minY) { bunny->y = minY; bunny->yv = -bunny->yv; }
            if (bunny->y > maxY) { bunny->y = maxY; bunny->yv = -bunny->yv; }
        }
    }

    void renderBunniesG2()
    {
        G2->begin();
        {
            G2->clear(Colour::Cyan);

            Bunny *bunny = bunnies;
            for (int i = 0; i < bunny_c; i++, bunny++)
            {
                G2->drawImage(btex, Kore::round(bunny->x), Kore::round(bunny->y));

                /*
                float iw = btex->width;
                float ih = btex->height;
                G2->drawScaledSubImage(
                    btex,
                    0, 0, iw, ih,
                    Kore::round(bunny->x), Kore::round(bunny->y), iw, ih
                );
                */
            }
        }
        G2->end();
    }

    static const u32 u_vp      = hash("u_vp");
    static const u32 u_texture = hash("u_texture");

    void renderBunniesGFX(Mat4& matrix, Gfx *gfx) {
        // GFX
        Gfx::Sprite s;
        s.region   = breg;
        s.origin   = { 0, 0 };
        s.colours  = { Colour::White };

        gfx->begin();
        {
            gfx->clear(Graphics4::ClearColorFlag, 0xff000044);

            Bunny *bunny = bunnies;
            for (int i = 0; i < bunny_c; i++, bunny++)
            {
                s.position = { Kore::round(bunny->x), Kore::round(bunny->y) };
                gfx->setProgram(Game::Assets::pr_posTexCol->state);
                gfx->setMatrix(u_vp, matrix);
                gfx->setTexture(u_texture, s.region->texture);
                gfx->draw(s);
            }
        }
        gfx->end();
    }
}

namespace Game
{
    // ----------------------------------------------------------------------------------------------
    // CONSTRUCTORS

    GameScene::GameScene()
    : Scene()
    {
        gfx    = GAME->gfx;
        camera = new OrthographicStaticCamera(GAME->virtualSize);

        // initialise bunnies
        G2   = new Kore::Graphics2::Graphics2(GAME->virtualSize.w, GAME->virtualSize.h);
        btex = new Texture("wabbit_alpha.png");
        breg = new TextureRegion(btex, {0,0,26,37}, {0,0});
        minX = 0;
        maxX = GAME->virtualSize.w - btex->width;
        minY = 0;
        maxY = GAME->virtualSize.h - btex->height;
    }

    GameScene::~GameScene()
    {
        delete G2;
        delete breg;
        delete btex;
        delete camera;
    }

    // ----------------------------------------------------------------------------------------------
    // Scene

    void GameScene::onEntry()
    {
        Random::init(((int)System::time()) * 1000);
        bunny_c  = 0;
        fpsTime  = 0;
        fpsCount = 0;
        ::moreBunnies(1000);
    }

    void GameScene::onExit()
    {
    }

    // ----------------------------------------------------------------------------------------------

    void GameScene::moreBunnies() {
        ::moreBunnies(1000);
    }
    void GameScene::lessBunnies() {
        ::lessBunnies();
    }

    void GameScene::update(const GameTime *time)
    {
        ::updateBunnies(time->deltaTimeF);

        fpsTime += time->deltaTimeF;
        fpsCount++;
        if (fpsTime > 1.0) {
            printf("%d @ %.2f FPS\n", bunny_c, (((float)fpsCount) / fpsTime));
            fpsTime  = 0;
            fpsCount = 0;
        }
    }

    // ----------------------------------------------------------------------------

    void GameScene::draw()
    {
        // G2
//        renderBunniesG2();

        // GFX
        auto matrix = camera->getViewProjectionMatrix();
        renderBunniesGFX(matrix, gfx);
    }

}
