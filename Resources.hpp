#ifndef RESOURCES_LOAD
#define RESOURCES_LOAD
#include <fstream>
#include <sstream>
#include "MOGL/MOGL.hpp"

void readFileContents(const std::string& filename, std::stringstream& ss)
{
    std::ifstream file;
    file.open(filename.c_str());
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
}

void loadAnimation(mogl::MultimediaOGL& mogl, const std::string& animation_name, const std::string& filename)
{
    std::ifstream animation_file;
    animation_file.open(filename.c_str());
    std::stringstream ss;
    readFileContents(filename, ss);
    mogl::SpriteAnimation animation;
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
    animation.texture = mogl.textures().get(texture_name);
    int x;
    for (int i = 0; i < num_frames; ++i)
    {
        ss >> x;
        animation.frame_order.push_back(x);
    }
    for (int i = 0; i < num_frames; ++i)
    {
        ss >> x;
        animation.frame_time.push_back(hum::Time::milliseconds(x));
    }
    mogl.spriteAnimations().load(animation_name, animation);
}

void loadResources(mogl::MultimediaOGL& mogl, const std::string& resources_filename)
{
    std::stringstream ss;
    readFileContents(resources_filename, ss);
    std::string word, TYPE;
    while (ss >> word)
    {
        if (word == "TEXTURES" or word == "ANIMATIONS" or word == "SOUNDS" or word == "MUSIC")
        {
            TYPE = word;
            ss >> word;
        }

        if (TYPE == "TEXTURES")
        {
            std::string file;
            ss >> file;
            mogl.textures().load(word, "res/textures/" + file);
        }
        else if (TYPE == "ANIMATIONS")
        {
            std::string file;
            ss >> file;
            loadAnimation(mogl, word, "res/animations/" + file);
        }
        else if (TYPE == "SOUNDS")
        {
            std::string file;
            ss >> file;
            mogl.sounds().load(word, "res/sounds/" + file);
        }
        else if (TYPE == "MUSIC")
        {
            std::string file;
            ss >> file;
            mogl.music().load(word, "res/music/" + file);
        }
    }
}
#endif
