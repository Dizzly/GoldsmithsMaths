#ifndef CURVE_H_INCLUDED
#define CURVE_H_INCLUDED

class Curve
{
public:
    void AddControlPoint(octet::vec3);
    void SetThickness(float thick);
    void SetResolution(float resolution);


    void Draw();
private:

};


#endif