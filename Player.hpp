#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <cmath>
#include "Hum2D/Hum2D.hpp"
#include "Hum2D/SFML.hpp"
#include "Bullet.hpp"
#include "Resources.hpp"
#include "math.hpp"

class Player : public h2d::Behaviour
{
public:
    Player()
    {
        if (s_vel == -1)
        {
            std::stringstream ss = readFileContents("res/config/Player.cfg");
            ss
                >> s_vel
                >> s_ms_shoot
                >> s_lives;
        }
    }
    void init() override
    {
        p_sfml = actor().game().getPlugin<h2d::SFMLPlugin>();
        h2d::SpriteAnimation* player_animation = p_sfml->spriteAnimations().get("player");
        p_sprite = actor().addBehaviour<h2d::AnimatedSprite>(player_animation);
        p_sprite->pause();
        p_sprite->sprite().setOrigin(24, 18);
        p_sprite->transform().rotation = -90;

        p_kinematic = actor().addBehaviour<h2d::Kinematic>();
        p_prev_rotation = 0;
        p_live = s_lives;

        h2d::Actor* helper = actor().game().makeActor();
        helper->transform() = actor().transform();
        p_helper_kinematic = helper->addBehaviour<h2d::Kinematic>();
        helper->transform().z = 1000;
        helper->transform().y -= 30;
        helper->transform().x -= 20;
        auto rect = helper->addBehaviour<h2d::Rectangle>(sf::Vector2f(40, 5), sf::Color::Transparent);
        rect->rectangle().setOutlineColor(sf::Color::Blue);
        rect->rectangle().setOutlineThickness(1);
        p_live_rectangle = helper->addBehaviour<h2d::Rectangle>(sf::Vector2f(40, 5), sf::Color::Green);
    }

    void fixedUpdate() override
    {
        if (isDead())
        {
            p_sprite->stop();
            p_kinematic->velocity().x = 0;
            p_kinematic->velocity().y = 0;
            p_kinematic->velocity().rotation = 0;

            p_helper_kinematic->velocity().x = p_kinematic->velocity().x;
            p_helper_kinematic->velocity().y = p_kinematic->velocity().y;
            return;
        }
        // Look at the mouse
        auto mouse = p_sfml->input().getMouseCurrentPosition();
        double x = mouse.x - actor().transform().x;
        double y = mouse.y - actor().transform().y;
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
        p_kinematic->velocity().rotation = delta / actor().game().fixedUpdateTime().asSeconds();
        p_prev_rotation = angleInDegrees;

        // Movement
        if (p_sfml->input().isKeyDown(sf::Keyboard::A))
        {
            p_kinematic->velocity().x = -s_vel * (not p_sfml->input().isKeyDown(sf::Keyboard::D));
        }
        else if (p_sfml->input().isKeyDown(sf::Keyboard::D))
        {
            p_kinematic->velocity().x = s_vel;
        }
        else
        {
            p_kinematic->velocity().x = 0;
        }

        if (p_sfml->input().isKeyDown(sf::Keyboard::W))
        {
            p_kinematic->velocity().y = -s_vel * (not p_sfml->input().isKeyDown(sf::Keyboard::S));
        }
        else if (p_sfml->input().isKeyDown(sf::Keyboard::S))
        {
            p_kinematic->velocity().y = s_vel;
        }
        else
        {
            p_kinematic->velocity().y = 0;
        }

        if (p_kinematic->velocity().x != 0 or p_kinematic->velocity().y != 0)
        {
            p_sprite->play();
        }
        else
        {
            p_sprite->pause();
        }

        // Shooting
        if (p_sfml->input().isMouseButtonDown(sf::Mouse::Left) and p_clock.getTime().asMilliseconds() >= s_ms_shoot)
        {
            auto bullet = actor().game().makeActor();
            double mod = sqrt(cuad(x) + cuad(y));
            x /= mod;
            y /= mod;
            bullet->addBehaviour<Bullet>(x, y);
            bullet->transform().x = actor().transform().x + 30 * x - 7 * y;
            bullet->transform().y = actor().transform().y + 30 * y + 7 * x;
            p_clock.reset();
        }

        p_helper_kinematic->velocity().x = p_kinematic->velocity().x;
        p_helper_kinematic->velocity().y = p_kinematic->velocity().y;

        sf::Listener::setPosition(actor().transform().x, actor().transform().y, 10);
    }

    void hit()
    {
        if (not isDead())
        {
            --p_live;
            p_live_rectangle->transform().scale_x = (double)p_live/s_lives;
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
    h2d::SFMLPlugin* p_sfml;
    h2d::AnimatedSprite* p_sprite;
    h2d::Kinematic* p_kinematic;
    h2d::Kinematic* p_helper_kinematic;
    h2d::Rectangle* p_live_rectangle;
    double p_prev_rotation;
    h2d::Clock p_clock;
};
double Player::s_vel = -1;
unsigned int Player::s_ms_shoot;
int Player::s_lives;
#endif
