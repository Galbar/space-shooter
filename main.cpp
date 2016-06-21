#include "hummingbird/hum.hpp"
#include "MOGL/MOGL.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "EnemySpawner.hpp"

int main()
{
    hum::Game g(60);
    g.addPlugin<hum::KinematicWorld>();
    mogl::MultimediaOGL* mogl = g.addPlugin<mogl::MultimediaOGL>(sf::VideoMode(1000, 1000), "Space Shooter");
    loadResources(*mogl, "res/resources.def");

    mogl->getCamera().setOrthogonal(0, -21, 21, 0);

    auto a = g.makeActor();
    auto player = a->addBehavior<Player>();
    a->transform().position.x = 10;
    a->transform().position.y = 10;

    g.addPlugin<EnemySpawner>(player);
    sf::Music* music = mogl->music().get("ambiance");
    music->setLoop(true);
    music->play();
    g.run();
    return 0;
}
