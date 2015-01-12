#ifndef EQUATION_H_INCLUDED
#define EQUATION_H_INCLUDED

class Equation
{
    public:
        struct InputParameters
        {
            float params[10];
            unsigned size;

            InputParameters(){}

            InputParameters(float par[], int s)
            {
                for (unsigned i = 0; i < s; i++)
                {
                    params[i] = par[i];
                }                
                size = s;
            }

        };

        struct ReturnValues
        {
            float returns[10];
            unsigned size;

            ReturnValues(){}

            ReturnValues(float ret[], int s)
            {
                for (unsigned i = 0; i < s; i++)
                {
                    returns[i] = ret[i];
                }
                size = s;
            }

        };


    typedef void(EquatFunc)(InputParameters*, ReturnValues*, float);

private:

    EquatFunc* equationFunc;
    InputParameters params;
    ReturnValues    retValues;

public:

    //Get/Set the arguments taken by the function
    InputParameters GetParameters()
    {
        return params;
    }

    void SetParameters(InputParameters par)
    {
        params = par;
    }

    //Set the actual functions that will be called
    void SetFunc(EquatFunc* func)
    {
        equationFunc = func;
    }

    //Get/Set the values returned by the equation, normally either 2 or 3
    ReturnValues GetRet()
    {
        return retValues;
    }

    //using the equation function, run it with these arguments and return values to this location
    //number of args and return values can be obtained with GetNumArgs, GetNumReturn.
    //T is a unique argument
    //0<T<infinity
    void Compute(float t)
    {
        equationFunc(&params,&retValues,t);
        //Calling the actual function pointer and returning the values
    }

    

};

#endif