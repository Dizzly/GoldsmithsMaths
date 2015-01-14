#ifndef TEXTURELINE_H_INCLUDED
#define TEXTURELINE_H_INCLUDED
#include "../../octet.h"
class TextureLine
{
public:
    TextureLine() :scale(1, 1)
    {
    
    }
    void Init()
    {
        octet::param_shader *shader = new octet::param_shader();
        shader->
    
    }
private:
    octet::ref<octet::material> mats_;
    octet::ref<octet::mesh> quadMesh_;
    
    octet::vec2 scale;
};




#endif