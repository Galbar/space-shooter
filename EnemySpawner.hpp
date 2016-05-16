#ifndef ENEMY_SPAWNER_HPP
#define ENEMY_SPAWNER_HPP
#include <cstdlib>
#include "hummingbird/hum.hpp"
#include "Enemy.hpp"

class Player;
class EnemySpawner : public hum::Plugin
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
        {
            auto enemy = game().makeActor();
            enemy->addBehavior<Enemy>(p_player);

            enemy->transform().position.x = rand() % 1000;
            enemy->transform().position.y = rand() % 1000;
            clk.reset();
        }
    }
private:
    Player* p_player;
    hum::Clock clk;
};
#endif
