#ifndef ENEMY_SPAWNER_HPP
#define ENEMY_SPAWNER_HPP
#include <cstdlib>
#include "Hum2D/Hum2D.hpp"
#include "Enemy.hpp"

class Player;
class EnemySpawner : public h2d::Plugin
{
public:
    EnemySpawner(Player* player):
    p_player(player)
    {}

    void preFixedUpdate() override
    {
        if (p_player->isDead())
        {
            return;
        }
        if (clk.getTime().asSeconds() > 0.5)
        //if (game().getPlugin<h2d::SFMLPlugin>()->input().isKeyPressed(sf::Keyboard::Space))
        {
            auto enemy = game().makeActor();
            enemy->addBehavior<Enemy>(p_player);

            enemy->transform().x = rand() % 1000;
            enemy->transform().y = rand() % 1000;
            clk.reset();
        }
    }
private:
    Player* p_player;
    h2d::Clock clk;
};
#endif
