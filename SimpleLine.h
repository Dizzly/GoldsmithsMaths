#ifndef SIMPLELINE_H_INCLUDED
#define SIMPLELINE_H_INCLUDED
//simple lines using GLLines, may require extending
class SimpleLine
{
public:

    ~SimpleLine(){}

    void Draw()
    {
        int size = points_.size()*sizeof(Vert);
        meshy_->allocate(size, 0);
        meshy_->set_params(sizeof(Vert), 0, points_.size(), GL_LINE_STRIP, 0);
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        meshy_->add_attribute(octet::attribute_color, 4, GL_UNSIGNED_BYTE, 12, TRUE);
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
    virtual void Init(int numberOfVertex, octet::mesh* mesh = nullptr)
    {
        if (mesh)
        {
            meshy_ = mesh;
        }
        points_.clear();
        if (numberOfVertex > points_.capacity())
        {
            points_.reserve(numberOfVertex);
        }
    }
    virtual void AddPoint(const octet::vec3& point,uint32_t color=~0) {
        assert(point.z() <= 0);
        points_.push_back(Vert(point, color));
    }
private:
    struct Vert{
        Vert(octet::vec3p v, uint32_t c)
        {
            vert = v, color = c;
        }
        octet::vec3p vert;
        uint32_t color;
    };

    uint32_t color_;
    octet::ref<octet::mesh> meshy_;
    std::vector<Vert> points_;

    
};



#endif
