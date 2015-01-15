#ifndef PRETTYLINE_H_INCLUDED
#define PRETTYLINE_H_INCLUDED

#include "Line.h"


//simple lines using GLLines, may require extending
class PrettyLine : public Line
{

private:

    void AddDrawingPoint(octet::vec3& p)
    {
        *vtxP_++ = p.x();
        *vtxP_++ = p.y();
        *vtxP_++ = p.z();
    }

    // (1-t)^3p0 + 3(1-t)^2tp1 + 3(1-t)t^2p2 + t^3p3 = p(x,y,z)
    octet::vec3 CalculateBezierPoint(float t, octet::vec3& p0, octet::vec3& p1, octet::vec3& p2, octet::vec3& p3)
    {
        float umt = (1 - t);
        float tq = t * t;
        float umtq = umt * umt;
        float umtc = umtq * umt;
        float tc = tq * t;

        return umtc * p0 + 3 * umtq * t * p1 + 3 * umt * tq * p2 + tc * p3;

    }

public:

    ~PrettyLine(){}

    void Draw()override
    {
        if (controlPoints_.size() < 4)
        {
            return;
        }

        octet::vec3 bz_point;

        octet::vec3 p0;
        octet::vec3 p1;
        octet::vec3 p2;
        octet::vec3 p3;

        

        //Calling Bezier Routine, all points must be already added to the Control Points
        for (unsigned i = 0; i < calculatedCurves_; i++)
        {

            octet::vec3 p0 = controlPoints_[i*4];
            octet::vec3 p1 = controlPoints_[i*4 + 1];
            octet::vec3 p2 = controlPoints_[i*4 + 2];
            octet::vec3 p3 = controlPoints_[i*4 + 3];

            //Handling first drawing separately
            if (i == 0)
            {
                bz_point = CalculateBezierPoint(0, p0, p1, p2, p3);
                AddDrawingPoint(bz_point);
            }

            for (unsigned j = 1; j <= drawingPrecision_; j++)
            {
                //Moving along the line
                float t = j / (float)drawingPrecision_;
                bz_point = CalculateBezierPoint(t, p0, p1, p2, p3);
                AddDrawingPoint(bz_point);
            }

        }
        float* v = vtxP_;
        float x = *v - 2;
        float y = *v - 1;
        float z = *v;

    }

    void SetDrawingPrecision(int precision)override
    {
        drawingPrecision_ = precision;
    }

    virtual void SetMesh(octet::mesh* mesh)
    {
        meshy_ = mesh;
    }

    virtual octet::mesh* GetMesh()
    {
        return meshy_;
    }

    //Number of Vertex will be the low resolution calculated from the line equation * bezier resolution
    virtual void Init(int numberOfVertex, octet::mesh* mesh = nullptr)
    {
        if (mesh)
        {
            meshy_ = mesh;
        }

        //Setting default precision
        drawingPrecision_ = 2;
        calculatedCurves_= std::floor((float)numberOfVertex / 4);
        int totalVertexes = drawingPrecision_ * calculatedCurves_; //counting segments between n points
        
        meshy_->allocate(totalVertexes * sizeof(float)* 3, 0);
        meshy_->set_params(sizeof(float)* 3, 0, totalVertexes, GL_LINE_STRIP, 0);

        controlPoints_.resize(0);
        controlPoints_.reserve(totalVertexes);
 
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        octet::gl_resource::wolock vl(meshy_->get_vertices());
        vtxP_ = (float*)vl.u8();
    }

    virtual void AddPoint(const octet::vec3& point)override
    {
        controlPoints_.push_back(point);
    }

private:

    octet::ref<octet::mesh> meshy_;
    octet::dynarray<octet::vec3> controlPoints_;
    float* vtxP_;
    int drawingPrecision_;
    int calculatedCurves_;//keeps track of how many we already used
};



#endif
