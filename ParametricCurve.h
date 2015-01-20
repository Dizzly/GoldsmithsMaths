#ifndef PARAMETRICCURVE_H_INCLUDED
#define PARAMETRICCURVE_H_INCLUDED

#include "../../octet.h"
#include "Equation.h"
#include "SimpleLine.h"
#include "Bezier.h"

//a curve class using parametric equations to define where points are
//the curve will draw from t=0->1 by default
//The parametric curve requires an equation to be defined, as there are many different parametric equations
class ParametricCurve
{
public:
    //default constructor, user can specify a new equation function
    ParametricCurve(Equation::EquatFunc func = Hypotrochoid,int paramNumber=3,int returnNumber=2, int lineType = 0)
    {
        maxResolution_ = 1000;
        thickness_ = 1;
        smallestAcceptableArea_ = 0.00001f;
        tProbeIncrement_ = 0.02f;

        line_ = new SimpleLine();
        
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

    void GenerateLine(const octet::vec3& pointA, float tA,const octet::vec3& pointB, float tB,float tMax)
    {
        /*
         Uses a triangle's area using two points, and the point between them
         to calculate the curviture near the 3 points
         */
        float midT = tA + ((tB - tA) / 2);
        equation_.Compute(midT);
        octet::vec3 pointMid = GetEquationReturn();
        float triSize = cross(pointA - pointMid, pointB - pointMid).length() / 2;
        if (triSize <= smallestAcceptableArea_)
        {
            line_->AddPoint(pointB);
            return;
        }
        else
        {
            GenerateLine(pointA, tA, pointMid, midT,tMax);
            GenerateLine(pointMid, midT, pointB, tB,tMax);
        }
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
        glPointSize(3);

        line_->Init((int)((maxT-minT)/tProbeIncrement_),m);


        equation_.Compute(minT);
        octet::vec3 start = GetEquationReturn();
        equation_.Compute(minT + tProbeIncrement_);
        octet::vec3 next = GetEquationReturn();

        bool done = false;
        float startT = minT;
        float nextT = minT + tProbeIncrement_;

        while (!done)
        {
            GenerateLine(start, startT, next, nextT, maxT);
            
            startT = nextT;
            nextT += tProbeIncrement_;
            if (nextT >= maxT)
            {
                done = true;
                break;
            }

            start = next;
            equation_.Compute(nextT);
            next = GetEquationReturn();
        }

       /* float t = minT;
        for (int i = 0; i < maxResolution_;++i)
        {
            equation_.Compute(t);
            line_->AddPoint(octet::vec3(equation_.GetRet().returns[0],
                equation_.GetRet().returns[1],
                0));
            t += tIncrement;
        }*/
        line_->Draw();
    }
private:
    octet::vec3 GetEquationReturn()
    {
        if (equation_.GetRet().size == 3)
        {
            return octet::vec3(equation_.GetRet().returns[0],
                equation_.GetRet().returns[1],
                equation_.GetRet().returns[2]);
        }
        return octet::vec3(equation_.GetRet().returns[0],
            equation_.GetRet().returns[1],
            0);
    }
private:
    float thickness_;
    int maxResolution_;
    Line* line_;
    Equation equation_;

    float tProbeIncrement_;

    float smallestAcceptableArea_;
};


#endif