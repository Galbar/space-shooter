#ifndef BULLET_HPP
#define BULLET_HPP
#include "Hum2D/Hum2D.hpp"
#include "Hum2D/SFML.hpp"
#include "Resources.hpp"
#include "math.hpp"

class Enemy;
class Bullet : public h2d::Behavior
{
public:
    Bullet(double comp_x, double comp_y):
    p_comp_x(comp_x),
    p_comp_y(comp_y)
    {
        if (s_vel == -1)
        {
            std::stringstream ss;
            readFileContents("res/config/Bullet.cfg", ss);
            ss >> s_vel;
        }
    }
    void init() override
    {
        h2d::SFMLPlugin* sfml = actor().game().getPlugin<h2d::SFMLPlugin>();
        p_explode = actor().addBehavior<h2d::AnimatedSprite>(sfml->spriteAnimations().get("bullet_explosion"));
        p_explode->disable();
        p_explode->setLooping(false);
        p_explode->sprite().setOrigin(12, 12);
        p_bullet = actor().addBehavior<h2d::Sprite>(*sfml->textures().get("sprites"), sf::IntRect(0, 96, 24, 24));
        p_bullet->sprite().setOrigin(12, 18);

        p_kinematic = actor().addBehavior<h2d::Kinematic>();
        p_kinematic->velocity().x = p_comp_x * s_vel;
        p_kinematic->velocity().y = p_comp_y * s_vel;

        double angleInRadians = std::atan2(p_comp_y, p_comp_x);
        double angleInDegrees = (angleInRadians / M_PI) * 180.0;
        p_bullet->transform().rotation = angleInDegrees - 90;

        sfml->sounds().play("gun_shot", 40, false, true);
        clk.reset();
    }

    void fixedUpdate() override
    {
        if (clk.getTime().asSeconds() > 0.5 and not p_explode->isEnabled())
        {
            p_bullet->disable();
            p_explode->enable();
            p_kinematic->velocity().x = 0;
            p_kinematic->velocity().y = 0;
        }
        else if (p_explode->status() == h2d::AnimatedSprite::STOPPED)
        {
            actor().game().destroy(actor());
        }
        else if (not p_explode->isEnabled())
        {
            auto& actors = actor().game().actors();
            double x, y, mod;
            for (auto it = actors.begin(); it != actors.end(); ++it)
            {
                h2d::Actor* a = *it;
                x = a->transform().x - actor().transform().x;
                y = a->transform().y - actor().transform().y;
                mod = sqrt(cuad(x) + cuad(y));
                if (mod < 20)
                {
                    try
                    {
                        a->getBehavior<Enemy>();
                    }
                    catch (h2d::exception::BehaviorNotFound e)
                    {
                        continue;
                    }
                    actor().game().destroy(*a);
                    p_bullet->disable();
                    p_explode->enable();
                    p_kinematic->velocity().x = 0;
                    p_kinematic->velocity().y = 0;
                    break;
                }
            }
        }
    }
private:
    static double s_vel;
    double p_comp_x, p_comp_y;
    h2d::Clock clk;
    h2d::Kinematic* p_kinematic;
    h2d::Sprite* p_bullet;
    h2d::AnimatedSprite* p_explode;
};
double Bullet::s_vel = -1;
#endif
