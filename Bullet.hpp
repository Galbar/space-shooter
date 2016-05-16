#ifndef BULLET_HPP
#define BULLET_HPP
#include "hummingbird/hum.hpp"
#include "MOGL/MOGL.hpp"
#include "Resources.hpp"
#include "math.hpp"

class Enemy;
class Bullet : public hum::Behavior
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
        mogl::MultimediaOGL* mogl = actor().game().getPlugin<mogl::MultimediaOGL>();
        p_explode = actor().addBehavior<mogl::AnimatedSprite>(mogl->spriteAnimations().get("bullet_explosion"));
        p_explode->disable();
        p_explode->setLooping(false);
        p_explode->setOrigin(hum::Vector3f(12./24., 12./24., 0));
        p_explode->transform().scale.x = 24.;
        p_explode->transform().scale.y = 24.;
        p_bullet = actor().addBehavior<mogl::Sprite>(mogl->textures().get("sprites"), sf::IntRect(0, 96, 24, 24));
        p_explode->setOrigin(hum::Vector3f(12./24., 18./24., 0));
        p_bullet->transform().scale.x = 24.;
        p_bullet->transform().scale.y = 24.;

        p_kinematic = actor().addBehavior<hum::Kinematic>();
        p_kinematic->velocity().position.x = p_comp_x * s_vel;
        p_kinematic->velocity().position.y = p_comp_y * s_vel;

        double angleInRadians = std::atan2(p_comp_y, p_comp_x);
        double angleInDegrees = (angleInRadians / M_PI) * 180.0;
        p_bullet->transform().rotation.z = angleInDegrees - 90;

        mogl->sounds().play("gun_shot", 40, false, true);
        clk.reset();
    }

    void fixedUpdate() override
    {
        if (clk.getTime().asSeconds() > 0.5 and not p_explode->isEnabled())
        {
            p_bullet->disable();
            p_explode->enable();
            p_kinematic->velocity().position.x = 0;
            p_kinematic->velocity().position.y = 0;
        }
        else if (p_explode->status() == mogl::AnimatedSprite::Status::STOPPED)
        {
            actor().game().destroy(actor());
        }
        else if (not p_explode->isEnabled())
        {
            auto& actors = actor().game().actors();
            double x, y, mod;
            for (auto it = actors.begin(); it != actors.end(); ++it)
            {
                hum::Actor* a = *it;
                x = a->transform().position.x - actor().transform().position.x;
                y = a->transform().position.y - actor().transform().position.y;
                mod = sqrt(cuad(x) + cuad(y));
                if (mod < 25)
                {
                    try
                    {
                        a->getBehavior<Enemy>();
                    }
                    catch (hum::exception::BehaviorNotFound e)
                    {
                        continue;
                    }
                    actor().game().destroy(*a);
                    p_bullet->disable();
                    p_explode->enable();
                    p_kinematic->velocity().position.x = 0;
                    p_kinematic->velocity().position.y = 0;
                    break;
                }
            }
        }
    }
private:
    static double s_vel;
    double p_comp_x, p_comp_y;
    hum::Clock clk;
    hum::Kinematic* p_kinematic;
    mogl::Sprite* p_bullet;
    mogl::AnimatedSprite* p_explode;
};
double Bullet::s_vel = -1;
#endif
