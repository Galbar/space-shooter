#include "Hum2D/Hum2D.hpp"
#include "Hum2D/SFML.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "EnemySpawner.hpp"

int main()
{
    h2d::Game g(60);
    g.addPlugin<h2d::KinematicWorld>();
    h2d::SFMLPlugin* sfml = g.addPlugin<h2d::SFMLPlugin>(1000, 1000, "juego");
    loadResources(*sfml, "res/resources.def");

    auto a = g.makeActor();
    auto player = a->addBehavior<Player>();
    a->transform().x = 200;
    a->transform().y = 200;

    g.addPlugin<EnemySpawner>(player);
    sf::Music* music = sfml->music().get("ambiance");
    music->setLoop(true);
    music->play();
    g.run();
    return 0;
}
