#ifndef LINE_H_INCLUDED
#define LINE_H_INCLUDED

#include "../../octet.h"
class Line
{
public:
    virtual ~Line() = 0{};

    virtual void SetMesh(octet::mesh* mesh){};
    virtual void GetMaterial(octet::material* mat){};

    virtual octet::mesh* GetMesh(){ return nullptr; };
    virtual octet::material* GetMaterial(){ return nullptr; };
    virtual void Init(int numberOfVertex, octet::mesh* mesh = nullptr) = 0;
    virtual void AddPoint(const octet::vec3& point) = 0;
    virtual void Draw() = 0;
private:
};

#endif