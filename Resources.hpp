#ifndef RESOURCES_LOAD
#define RESOURCES_LOAD
#include <fstream>
#include <sstream>
#include "Hum2D/SFML.hpp"

std::stringstream readFileContents(const std::string& filename)
{
    std::ifstream file;
    file.open(filename.c_str());
    std::stringstream ss;
    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] != '#')
        {
            ss << line;
            ss << "\n";
        }
    }
    file.close();
    return ss;
}

void loadAnimation(h2d::SFMLPlugin& sfml, const std::string& animation_name, const std::string& filename)
{
    std::ifstream animation_file;
    animation_file.open(filename.c_str());
    std::stringstream ss = readFileContents(filename);
    h2d::SpriteAnimation animation;
    std::string texture_name;
    int num_frames;
    ss
        >> texture_name
        >> animation.offset_x
        >> animation.offset_y
        >> animation.margin_x
        >> animation.margin_y
        >> animation.width
        >> animation.height
        >> num_frames;
    animation.texture = sfml.textures().get(texture_name);
    int x;
    for (int i = 0; i < num_frames; ++i)
    {
        ss >> x;
        animation.frame_order.push_back(x);
    }
    for (int i = 0; i < num_frames; ++i)
    {
        ss >> x;
        animation.frame_time.push_back(h2d::Time::milliseconds(x));
    }
    sfml.spriteAnimations().load(animation_name, animation);
}

void loadResources(h2d::SFMLPlugin& sfml, const std::string& resources_filename)
{
    std::stringstream ss = readFileContents(resources_filename);
    std::string word, TYPE;
    while (ss >> word)
    {
        if (word == "TEXTURES" or word == "ANIMATIONS")
        {
            TYPE = word;
            ss >> word;
        }

        if (TYPE == "TEXTURES")
        {
            std::string file;
            ss >> file;
            sfml.textures().load(word, "res/textures/" + file);
        }
        else if (TYPE == "ANIMATIONS")
        {
            std::string file;
            ss >> file;
            loadAnimation(sfml, word, "res/animations/" + file);
        }
    }
}
#endif
