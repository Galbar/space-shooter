#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "Hum2D/Hum2D.hpp"
#include "Hum2D/SFML.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "math.hpp"

class Enemy : public h2d::Behaviour
{
public:
    enum State { WALKING, HITTING, WAITING, WIN, DONE };
    Enemy(Player* player):
    p_player(player)
    {
        if (s_vel == -1)
        {
            std::stringstream ss = readFileContents("res/config/Enemy.cfg");
            ss >> s_vel;
            ss >> s_min_player_dist;
        }
    }

    void init() override
    {
        p_kinematic = &actor().addBehaviour<h2d::Kinematic>();
        p_prev_rotation = 0;

        p_sfml = actor().game().getPlugin<h2d::SFMLPlugin>();
        p_sprite = &actor().addBehaviour<h2d::AnimatedSprite>(p_sfml->spriteAnimations().get("enemy_walking"));
        p_sprite->sprite().setOrigin(24, 18);
        p_sprite->pause();
        actor().transform().rotation = -90;

        p_blood = &actor().addBehaviour<h2d::AnimatedSprite>(p_sfml->spriteAnimations().get("enemy_attack1_blood"));
        p_blood->sprite().setOrigin(24, -17);
        p_blood->setLooping(false);
        p_blood->stop();
        p_blood->disable();

        p_status = WAITING;
    }

    void fixedUpdate() override
    {
        if (p_status == DONE)
        {
            return;
        }

        if (p_player->isDead() and p_status != DONE and p_status != WIN)
        {
            p_status = WIN;
            p_sprite->stop();
            p_blood->disable();
            p_clock.reset();
        }

        // Look at the player
        double x = p_player->actor().transform().x - actor().transform().x;
        double y = p_player->actor().transform().y - actor().transform().y;
        double angleInRadians = std::atan2(y, x);
        double angleInDegrees = (angleInRadians / M_PI) * 180.0;
        double delta = angleInDegrees - p_prev_rotation;
        if (delta > 180)
        {
            delta -= 360;
        }
        else if (delta < -180)
        {
            delta += 360;
        }
        p_kinematic->rotation_velocity = delta / actor().game().fixedUpdateTime().asSeconds();
        p_prev_rotation = angleInDegrees;

        p_kinematic->velocity_x = 0;
        p_kinematic->velocity_y = 0;

        double mod = sqrt(cuad(x) + cuad(y));
        if (p_status == WIN)
        {
            if (p_clock.getTime().asSeconds() > 1)
            {
                p_sprite->setSpriteAnimation(p_sfml->spriteAnimations().get("enemy_win"));
                p_sprite->setLooping(false);
                p_sprite->play();
                p_status = DONE;
            }
        }
        else if (p_status == WALKING and s_min_player_dist < mod)
        {
            x /= mod;
            y /= mod;
            p_kinematic->velocity_x = x * s_vel;
            p_kinematic->velocity_y = y * s_vel;
            if (p_sprite->status() != h2d::AnimatedSprite::PLAYING)
            {
                p_sprite->play();
            }
        }
        else
        {
            if (p_status != WAITING)
            {
                if (p_status != HITTING)
                {
                    p_sprite->setSpriteAnimation(p_sfml->spriteAnimations().get("enemy_attack1"));
                    p_sprite->play();
                    p_sprite->setLooping(false);
                    p_status = HITTING;
                }
                else
                {
                    if (p_sprite->status() == h2d::AnimatedSprite::STOPPED)
                    {
                        p_sprite->setSpriteAnimation(p_sfml->spriteAnimations().get("enemy_walking"));
                        p_sprite->setLooping(true);
                        p_status = WAITING;

                        p_blood->stop();
                        p_blood->disable();

                        p_clock.reset();
                    }
                    else if (p_sprite->frameIndex() > 1 and not p_blood->isEnabled())
                    {
                        if (mod < s_min_player_dist)
                        {
                            p_blood->enable();
                            p_blood->play();
                            p_player->hit();
                        }
                    }
                }
            }
            else
            {
                h2d::Time t = p_clock.getTime();
                if (t.asSeconds() > 1)
                {
                    p_status = WALKING;
                }
            }
        }
    }

    static const char* behaviourName()
    {
        return "Enemy";
    }

private:
    static double s_vel;
    static double s_min_player_dist;
    h2d::Clock p_clock;
    State p_status;
    Player* p_player;
    h2d::Kinematic* p_kinematic;
    double p_prev_rotation;
    h2d::AnimatedSprite* p_sprite;
    h2d::AnimatedSprite* p_blood;
    h2d::SFMLPlugin* p_sfml;
};
double Enemy::s_vel = -1;
double Enemy::s_min_player_dist = -1;
#endif
