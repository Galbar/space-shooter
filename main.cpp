#include "Hum2D/Hum2D.hpp"
#include "Hum2D/SFML.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "EnemySpawner.hpp"

int main()
{
    h2d::Game g(60);
    h2d::SFMLPlugin sfml(1000, 1000, "juego");
    loadResources(sfml, "res/resources.def");
    g.addPlugin(sfml);

    h2d::KinematicWorld kinematic_world;
    g.addPlugin(kinematic_world);

    auto& a = g.makeActor();
    auto& player = a.addBehaviour<Player>();
    a.transform().x = 200;
    a.transform().y = 200;

    EnemySpawner spawner(&player);
    g.addPlugin(spawner);

    g.run();
    return 0;
}
