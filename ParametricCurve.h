#ifndef PARAMETRICCURVE_H_INCLUDED
#define PARAMETRICCURVE_H_INCLUDED

#include "../../octet.h"
#include "Equation.h"
#include "SimpleLine.h"
#include "PrettyLine.h"

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
        line_ = new PrettyLine();
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

        line_->Init(maxResolution_,m);

        float t = minT;
        for (int i = 0; i < maxResolution_;++i)
        {
            equation_.Compute(t);
            line_->AddPoint(octet::vec3(equation_.GetRet().returns[0],
                equation_.GetRet().returns[1],
                0));
            t += tIncrement;
        }
        line_->Draw();
    }
private:
    float thickness_;
    int maxResolution_;
    Line* line_;
    Equation equation_;
};


#endif