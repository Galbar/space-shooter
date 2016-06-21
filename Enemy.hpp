#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "hummingbird/hum.hpp"
#include "MOGL/MOGL.hpp"
#include "Resources.hpp"
#include "Player.hpp"
#include "math.hpp"

class Enemy : public hum::Behavior
{
private:
    enum State { WALKING, HITTING, WAITING, WIN, DONE };

    static float s_vel;
    static float s_min_player_dist;

    hum::Clock p_clock;
    State p_status;
    Player* p_player;
    hum::Kinematic* p_kinematic;
    float p_prev_rotation;
    mogl::AnimatedSprite* p_sprite;
    mogl::AnimatedSprite* p_blood;
    mogl::MultimediaOGL* p_mogl;
    mogl::SoundId p_sound;

public:
    Enemy(Player* player):
    p_player(player),
    p_sound(0)
    {
        if (s_vel == -1)
        {
            std::stringstream ss;
            readFileContents("res/config/Enemy.cfg", ss);
            ss >> s_vel;
            ss >> s_min_player_dist;
        }
    }

    void init() override
    {
        p_kinematic = actor().addBehavior<hum::Kinematic>();
        p_prev_rotation = 0;

        p_mogl = actor().game().getPlugin<mogl::MultimediaOGL>();
        p_sprite = actor().addBehavior<mogl::AnimatedSprite>(p_mogl->spriteAnimations().get("enemy_walking"));
        p_sprite->pause();
        p_sprite->setOrigin(hum::Vector3f(24./48., 18./48., 0));
        actor().transform().rotation.z = -90;

        p_blood = actor().addBehavior<mogl::AnimatedSprite>(p_mogl->spriteAnimations().get("enemy_attack1_blood"));
        p_blood->setLooping(false);
        p_blood->stop();
        p_blood->disable();
        p_blood->setOrigin(hum::Vector3f(24./48., -10./24., 0));

        p_status = WAITING;
    }

    void onDestroy() override
    {
        if (p_mogl->sounds().get(p_sound) != nullptr)
        {
            p_mogl->sounds().get(p_sound)->stop();
        }
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
        float x = p_player->actor().transform().position.x - actor().transform().position.x;
        float y = p_player->actor().transform().position.y - actor().transform().position.y;
        float angleInRadians = std::atan2(y, x);
        float angleInDegrees = (angleInRadians / M_PI) * 180.0;
        float delta = angleInDegrees - p_prev_rotation;
        if (delta > 180)
        {
            delta -= 360;
        }
        else if (delta < -180)
        {
            delta += 360;
        }
        p_kinematic->velocity().rotation.z = delta / actor().game().fixedUpdateTime().asSeconds();
        p_prev_rotation = angleInDegrees;

        p_kinematic->velocity().position.x = 0;
        p_kinematic->velocity().position.y = 0;

        float mod = sqrt(square(x) + square(y));
        if (p_status == WIN)
        {
            if (p_clock.getTime().asSeconds() > 1)
            {
                p_sprite->setSpriteAnimation(p_mogl->spriteAnimations().get("enemy_win"));
                p_sprite->setLooping(false);
                p_sprite->play();
                p_status = DONE;
            }
        }
        else if (p_status == WALKING and s_min_player_dist < mod)
        {
            x /= mod;
            y /= mod;
            p_kinematic->velocity().position.x = x * s_vel;
            p_kinematic->velocity().position.y = y * s_vel;
            if (p_sprite->status() != mogl::AnimatedSprite::Status::PLAYING)
            {
                p_sprite->play();
            }

            if (p_mogl->sounds().get(p_sound) == nullptr and (rand()%100) > 99)
            {
                auto info = p_mogl->sounds().play("roar", 1000);
                info.second->setAttenuation(0.1);
                p_sound = info.first;
            }
        }
        else
        {
            if (p_status != WAITING)
            {
                if (p_status != HITTING) // state == WALKING and player is in hitting distance
                {
                    p_sprite->setSpriteAnimation(p_mogl->spriteAnimations().get("enemy_attack1"));
                    p_sprite->play();
                    p_sprite->setLooping(false);
                    p_status = HITTING;
                    sf::Sound* sound = p_mogl->sounds().play("enemy_attack", 50).second;
                    sound->setPosition(actor().transform().position.x, actor().transform().position.y, 0);
                    sound->setAttenuation(0.01);
                }
                else
                {
                    if (p_sprite->status() == mogl::AnimatedSprite::Status::STOPPED)
                    {
                        p_sprite->setSpriteAnimation(p_mogl->spriteAnimations().get("enemy_walking"));
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
                hum::Time t = p_clock.getTime();
                if (t.asSeconds() > 1)
                {
                    p_status = WALKING;
                }
            }
        }
        sf::Sound* sound = p_mogl->sounds().get(p_sound);
        if (sound != nullptr)
        {
            sound->setPosition(actor().transform().position.x, actor().transform().position.y, 0);
        }
    }

    static const char* behaviorName()
    {
        return "Enemy";
    }
};
float Enemy::s_vel = -1;
float Enemy::s_min_player_dist = -1;
#endif
