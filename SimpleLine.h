#ifndef SIMPLELINE_H_INCLUDED
#define SIMPLELINE_H_INCLUDED

#include "Line.h"


//simple lines using GLLines, may require extending
class SimpleLine: public Line
{
public:

    ~SimpleLine(){}

    void Draw()override
    {
        
    }
    
    virtual void SetMesh(octet::mesh* mesh)
    {
        meshy_ = mesh;
    }
    virtual octet::mesh* GetMesh()
    {
        return meshy_;
    }
    virtual void Init(int numberOfVertex, octet::mesh* mesh=nullptr)
    {
        if (mesh)
        {
            meshy_ = mesh;
        }
        meshy_->allocate(numberOfVertex*sizeof(float)* 3, 0);
        meshy_->set_params(sizeof(float)* 3, 0, numberOfVertex, GL_LINE_STRIP, 0);
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        octet::gl_resource::wolock vl(meshy_->get_vertices());
        vtxP_ = (float*)vl.u8();
    }
    virtual void AddPoint(const octet::vec3& point)override {
        *vtxP_++ = point.x();
        *vtxP_++ = point.y();
        *vtxP_++ = point.z();
    }
private:
    octet::ref<octet::mesh> meshy_;
    float * vtxP_;
};



#endif
