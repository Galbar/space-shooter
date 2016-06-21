#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "hummingbird/hum.hpp"
#include "MOGL/MOGL.hpp"
#include "Bullet.hpp"
#include "Resources.hpp"
#include "math.hpp"

class Player : public hum::Behavior
{
private:
    static int s_lives;
    static float s_vel;
    static unsigned int s_ms_shoot;

    int p_live;
    mogl::MultimediaOGL* p_mogl;
    mogl::AnimatedSprite* p_sprite;
    hum::Kinematic* p_kinematic;
    hum::Kinematic* p_helper_kinematic;
    mogl::Rectangle* p_live_rectangle;
    float p_prev_rotation;
    hum::Clock p_clock;

public:
    Player()
    {
        if (s_vel == -1)
        {
            std::stringstream ss;
            readFileContents("res/config/Player.cfg", ss);
            ss
                >> s_vel
                >> s_ms_shoot
                >> s_lives;
        }
    }
    void init() override
    {
        // get the MOGL plugin instance and store it
        p_mogl = actor().game().getPlugin<mogl::MultimediaOGL>();

        // get the SpriteAnimation, that has already been loaded before
        // game start, and create the AnimatedSprite with it.
        mogl::SpriteAnimation* player_animation =
            p_mogl->spriteAnimations().get("player");
        p_sprite =
            actor().addBehavior<mogl::AnimatedSprite>(player_animation);
        // Set the animation to pause as the player is not moving.
        p_sprite->pause();
        // fix the rotation of the sprite.
        p_sprite->transform().rotation.z = -90;

        // Set the center for transformations of the sprite to the center
        // of the astronaut tile (a bit displaced from the actual center
        // of the 48 by 48 tile)
        p_sprite->setOrigin(hum::Vector3f(24./48., 18./48., 0));

        // Add kinematic behavior to the player actor and store a pointer
        // to it
        p_kinematic = actor().addBehavior<hum::Kinematic>();

        // set other useful information
        p_prev_rotation = 0;
        p_live = s_lives;

        // Create the helper actor we don't need to save a pointer to it,
        // as we will mainly work with its Kinematic Behavior and said
        // behavior has a reference to its actor.
        hum::Actor* helper = actor().game().makeActor();
        // Sync the transformations
        helper->transform() = actor().transform();
        // Add Kinematic, save it and sync it with the player's one.
        p_helper_kinematic = helper->addBehavior<hum::Kinematic>();
        helper->transform().position.z = -0.5;
        helper->transform().position.y -= 0.8;
        helper->transform().position.x -= 0.4;

        // Add the background of the health bar
        auto rect =
            helper->addBehavior<mogl::Rectangle>(sf::Color::White);
        rect->transform().scale = hum::Vector3f(0.8, 0.1, 0.02);

        // Add the foreground color of the health bar (the actual
        // indicator)
        p_live_rectangle =
            helper->addBehavior<mogl::Rectangle>(sf::Color::Green);
        p_live_rectangle->transform().scale =
            hum::Vector3f(0.8, 0.1, 0.02);
        p_live_rectangle->transform().position.z = -0.1;
    }

    void fixedUpdate() override
    {
        if (isDead())
        {
            p_sprite->stop();
            p_kinematic->velocity().position.x = 0;
            p_kinematic->velocity().position.y = 0;
            p_kinematic->velocity().rotation.z = 0;

            p_helper_kinematic->velocity().position.x =
                p_kinematic->velocity().position.x;
            p_helper_kinematic->velocity().position.y =
                p_kinematic->velocity().position.y;
            return;
        }
        // Look at the mouse
        hum::Vector2f mouse = p_mogl->input().getMouseCurrentPosition();
        mouse /= 48.f;
        float x = mouse.x - actor().transform().position.x;
        float y = mouse.y - actor().transform().position.y;
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
        p_kinematic->velocity().rotation.z =
            delta / actor().game().fixedUpdateTime().asSeconds();
        p_prev_rotation = angleInDegrees;

        // Movement
        if (p_mogl->input().isKeyDown(sf::Keyboard::A))
        {
            p_kinematic->velocity().position.x =
                -s_vel * (not p_mogl->input().isKeyDown(sf::Keyboard::D));
        }
        else if (p_mogl->input().isKeyDown(sf::Keyboard::D))
        {
            p_kinematic->velocity().position.x = s_vel;
        }
        else
        {
            p_kinematic->velocity().position.x = 0;
        }

        if (p_mogl->input().isKeyDown(sf::Keyboard::W))
        {
            p_kinematic->velocity().position.y =
                -s_vel * (not p_mogl->input().isKeyDown(sf::Keyboard::S));
        }
        else if (p_mogl->input().isKeyDown(sf::Keyboard::S))
        {
            p_kinematic->velocity().position.y = s_vel;
        }
        else
        {
            p_kinematic->velocity().position.y = 0;
        }

        if (p_kinematic->velocity().position.x != 0
            or p_kinematic->velocity().position.y != 0)
        {
            p_sprite->play();
        }
        else
        {
            p_sprite->pause();
        }

        // Shooting
        if (p_mogl->input().isMouseButtonDown(sf::Mouse::Left)
            and p_clock.getTime().asMilliseconds() >= s_ms_shoot)
        {
            auto bullet = actor().game().makeActor();
            float mod = sqrt(square(x) + square(y));
            x /= mod;
            y /= mod;
            bullet->addBehavior<Bullet>(x, y);
            bullet->transform().position.x =
                actor().transform().position.x + 0.6 * x - 0.14 * y;
            bullet->transform().position.y =
                actor().transform().position.y + 0.6 * y + 0.14 * x;
            p_clock.reset();
        }

        p_helper_kinematic->velocity().position.x =
            p_kinematic->velocity().position.x;
        p_helper_kinematic->velocity().position.y =
            p_kinematic->velocity().position.y;

        sf::Listener::setPosition(
            actor().transform().position.x,
            actor().transform().position.y,
            10);
    }

    void hit()
    {
        if (not isDead())
        {
            --p_live;
            p_live_rectangle->transform().scale.x =
                (float)p_live/s_lives * 0.8;
        }
    }

    bool isDead() const
    {
        return (p_live <= 0);
    }
};
float Player::s_vel = -1;
unsigned int Player::s_ms_shoot;
int Player::s_lives;
#endif
