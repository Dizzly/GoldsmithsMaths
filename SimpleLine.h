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
        int size = points_.size()*sizeof(float)* 3;
        meshy_->allocate(size, 0);
        meshy_->set_params(sizeof(float)* 3, 0, points_.size(), GL_LINE_STRIP, 0);
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        octet::gl_resource::wolock vl(meshy_->get_vertices());
        float *vtxP = (float*)vl.u8();
        memcpy(vtxP, points_.data(), size);
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
        points_.clear();
        points_.reserve(numberOfVertex);
    }
    virtual void AddPoint(const octet::vec3& point)override {
        assert(point.z() <= 0);
        points_.push_back(point);
    }
private:
    octet::ref<octet::mesh> meshy_;
    std::vector<octet::vec3p> points_;
};



#endif
