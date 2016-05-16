#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <cmath>
#include "hummingbird/hum.hpp"
#include "MOGL/MOGL.hpp"
#include "Bullet.hpp"
#include "Resources.hpp"
#include "math.hpp"

class Player : public hum::Behavior
{
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
        p_mogl = actor().game().getPlugin<mogl::MultimediaOGL>();
        mogl::SpriteAnimation* player_animation = p_mogl->spriteAnimations().get("player");
        p_sprite = actor().addBehavior<mogl::AnimatedSprite>(player_animation);
        p_sprite->pause();
        p_sprite->transform().rotation.z = -90;
        p_sprite->setOrigin(hum::Vector3f(24./48., 18./48., 0));
        p_sprite->transform().scale.x = 48.;
        p_sprite->transform().scale.y = 48.;

        p_kinematic = actor().addBehavior<hum::Kinematic>();
        p_prev_rotation = 0;
        p_live = s_lives;

        hum::Actor* helper = actor().game().makeActor();
        helper->transform() = actor().transform();
        p_helper_kinematic = helper->addBehavior<hum::Kinematic>();
        helper->transform().position.z = -0.5;
        helper->transform().position.y -= 30;
        helper->transform().position.x -= 20;
        auto rect = helper->addBehavior<mogl::Rectangle>(sf::Color::White);
        rect->transform().scale = hum::Vector3f(40, 5, 1);
        p_live_rectangle = helper->addBehavior<mogl::Rectangle>(sf::Color::Green);
        p_live_rectangle->transform().scale = hum::Vector3f(40, 5, 1);
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

            p_helper_kinematic->velocity().position.x = p_kinematic->velocity().position.x;
            p_helper_kinematic->velocity().position.y = p_kinematic->velocity().position.y;
            return;
        }
        // Look at the mouse
        auto mouse = p_mogl->input().getMouseCurrentPosition();
        double x = mouse.x - actor().transform().position.x;
        double y = mouse.y - actor().transform().position.y;
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
        p_kinematic->velocity().rotation.z = delta / actor().game().fixedUpdateTime().asSeconds();
        p_prev_rotation = angleInDegrees;

        // Movement
        if (p_mogl->input().isKeyDown(sf::Keyboard::A))
        {
            p_kinematic->velocity().position.x = -s_vel * (not p_mogl->input().isKeyDown(sf::Keyboard::D));
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
            p_kinematic->velocity().position.y = -s_vel * (not p_mogl->input().isKeyDown(sf::Keyboard::S));
        }
        else if (p_mogl->input().isKeyDown(sf::Keyboard::S))
        {
            p_kinematic->velocity().position.y = s_vel;
        }
        else
        {
            p_kinematic->velocity().position.y = 0;
        }

        if (p_kinematic->velocity().position.x != 0 or p_kinematic->velocity().position.y != 0)
        {
            p_sprite->play();
        }
        else
        {
            p_sprite->pause();
        }

        // Shooting
        if (p_mogl->input().isMouseButtonDown(sf::Mouse::Left) and p_clock.getTime().asMilliseconds() >= s_ms_shoot)
        {
            auto bullet = actor().game().makeActor();
            double mod = sqrt(cuad(x) + cuad(y));
            x /= mod;
            y /= mod;
            bullet->addBehavior<Bullet>(x, y);
            bullet->transform().position.x = actor().transform().position.x + 30 * x - 7 * y;
            bullet->transform().position.y = actor().transform().position.y + 30 * y + 7 * x;
            p_clock.reset();
        }

        p_helper_kinematic->velocity().position.x = p_kinematic->velocity().position.x;
        p_helper_kinematic->velocity().position.y = p_kinematic->velocity().position.y;

        sf::Listener::setPosition(actor().transform().position.x, actor().transform().position.y, 10);
    }

    void hit()
    {
        if (not isDead())
        {
            --p_live;
            p_live_rectangle->transform().scale.x = (double)p_live/s_lives * 40.;
        }
    }

    bool isDead() const
    {
        return (p_live <= 0);
    }

private:
    static int s_lives;
    static double s_vel;
    static unsigned int s_ms_shoot;

    int p_live;
    mogl::MultimediaOGL* p_mogl;
    mogl::AnimatedSprite* p_sprite;
    hum::Kinematic* p_kinematic;
    hum::Kinematic* p_helper_kinematic;
    mogl::Rectangle* p_live_rectangle;
    double p_prev_rotation;
    hum::Clock p_clock;
};
double Player::s_vel = -1;
unsigned int Player::s_ms_shoot;
int Player::s_lives;
#endif
