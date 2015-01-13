#ifndef PARAMETRICCURVE_H_INCLUDED
#define PARAMETRICCURVE_H_INCLUDED

#include "../../octet.h"
#include "Equation.h"


//a curve class using parametric equations to define where points are
//the curve will draw from t=0->1 by default
//The parametric curve requires an equation to be defined, as there are many different parametric equations
class ParametricCurve
{
public:
    //default constructor, user can specify a new equation function
    ParametricCurve(Equation::EquatFunc func = Hypotrochoid,int paramNumber=3,int returnNumber=2)
    {
        maxResolution_ = 1000;
        thickness_ = 1;
        equation_.SetFunc(func,paramNumber,returnNumber);
    }

    void SetEquation(Equation::EquatFunc func, int paramNumber, int returnNumber)
    {
        equation_.SetFunc(func, paramNumber, returnNumber);
    }

    void SetParameters(Equation::InputParameters param)
    {
        equation_.SetParameters(param);
    }

    void SetThickness(float thick){ thickness_ = thick; }
    void SetMaxResolution(int resolution){maxResolution_ = resolution;}

    //TODO put all draw specific stuff into a line class
    void Draw(octet::mesh* m, float maxT = 1, float minT = 0){
        if (maxT <= 0 || maxT < minT)
        {
            return;
        }
        //max resolution is how many lines we are allowed to draw with
        //more detailed resolution stuff can be done here, straight lines using less lines to make them
        float tIncrement = maxT / maxResolution_;
        
        glLineWidth(thickness_);

        //setting up the mesh
        //allocating the total space we need
        m->allocate(sizeof(float)*3 *maxResolution_, 0);
       
        //defining how large are information is and how we choose to inerperate it
        m->set_params(sizeof(float)* 3, 0, maxResolution_, GL_LINE_STRIP, 0);
       
        //clear the attributes from the last time we used it
        m->clear_attributes();
        
        //and add an attribute for the position of the vertex
        m->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);

        octet::gl_resource::wolock vl(m->get_vertices());
        float* vtx = (float*)vl.u8();

        float t = minT;
        for (int i = 0; i < maxResolution_;++i)
        {
            equation_.Compute(t);
            *vtx++ = equation_.GetRet().returns[0];
            *vtx++ = equation_.GetRet().returns[1];
            *vtx++ = 0;
            t += tIncrement;
        }
    }
private:
    float thickness_;
    int maxResolution_;

    Equation equation_;
};


#endif