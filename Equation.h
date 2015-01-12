#ifndef EQUATION_H_INCLUDED
#define EQUATION_H_INCLUDED

class Equation
{
typedef void(EquatFunc)(float*, float*, float);
public:
    //returns the number of arguments taken by the function
    int GetNumArgs();
    //returns the number of values returned by the equation, normally either 2 or 3
    int GetNumReturn();

    //using the equation function, run it with these arguments and return values to this location
    //number of args and return values can be obtained with GetNumArgs, GetNumReturn.
    //T is a unique argument
    //0<T<infinity
    void Compute(float* args, float* ret, float t);

    void SetFunc(EquatFunc*);

private:
    //vars

    EquatFunc* equationFunc;
};

#endif