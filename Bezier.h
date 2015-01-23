#ifndef Bezier_H_INCLUDED
#define Bezier_H_INCLUDED

#include "Line.h"


//simple lines using GLLines, may require extending
class Bezier
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

    ~Bezier(){}
    Bezier()
    {
        mat = 0;
        selected_mat = 0;
        numControlPoints = 0;
    }
    void Draw()
    {
        if (numControlPoints < 4)
        {
            return;
        }

        //Allocating mesh

        int sizenode = numControlPoints;
        int points = curves * drawingPrecision_ + curves;
        int size = points * sizeof(float) * 3;
        meshy_->allocate(size, 0);
        meshy_->set_params(sizeof(float) * 3, 0, points, GL_LINE_STRIP, 0);
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        octet::gl_resource::wolock vl(meshy_->get_vertices());
        vtxP_ = (float*)vl.u8();

        octet::vec3 bz_point;
       
        for (unsigned i = 0; i < numControlPoints - 3; i+=3)
        {
            octet::vec3 p0 = spheres_[i]->get_node()->access_nodeToParent()[3].xyz();
            octet::vec3 p1 = spheres_[i + 1]->get_node()->access_nodeToParent()[3].xyz();
            octet::vec3 p2 = spheres_[i + 2]->get_node()->access_nodeToParent()[3].xyz();
            octet::vec3 p3 = spheres_[i + 3]->get_node()->access_nodeToParent()[3].xyz();

            for (unsigned j = 0; j <= drawingPrecision_; j++)
            {
                //Moving along the line
                float t = j / (float)drawingPrecision_;
                bz_point = CalculateBezierPoint(t, p0, p1, p2, p3);
                AddDrawingPoint(bz_point);
            }

        }

        
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
    virtual void Init(octet::visual_scene* sc, octet::mesh* mesh = nullptr)
    {
        if (mesh)
        {
            meshy_ = mesh;
        }
        if (!mat)
        {
            mat = new octet::material(octet::vec4(0, 1, 0, 1));
        }
        if (!selected_mat)
        {
            selected_mat = new octet::material(octet::vec4(1, 0, 0, 1));
        }
        if (!sphereMesh_)
        {
            sphereMesh_ = new octet::mesh_sphere(octet::vec3(0, 0, 0), 0.5f);
        }
        
        //octet::gl_resource::wolock vl(meshy_->get_vertices());
        //vtxP_ = (float*)vl.u8();

        if (spheres_.size() < 4)
        {
            for (int i = 0; i < 4; ++i)
            {
                spheres_.push_back(new octet::mesh_instance(new octet::scene_node(),
                    sphereMesh_, mat));
                sc->add_scene_node(spheres_[i]->get_node());
                sc->add_mesh_instance(spheres_[i]);
            }
        }
        
        Reset();

        //if (nodes_.size() == 0)
        {
            AddControlPoint(octet::vec3(rand.get(-12, -8), rand.get(-12, -8), 0), sc);
            AddControlPoint(octet::vec3(rand.get(-9, -3), rand.get(-2, 2), 0), sc);
            AddControlPoint(octet::vec3(rand.get(-3, 2), rand.get(0, 4), 0), sc);
            AddControlPoint(octet::vec3(rand.get(6, 10), rand.get(-7, -3), 0), sc);
        }

        curves = 1;
        visibility = true;
        /*else
        {
            nodes_[0]->access_nodeToParent()[3] = octet::vec4(-10, -10, 0, 1);
            nodes_[1]->access_nodeToParent()[3] = octet::vec4(-7, 0, 0, 1);
            nodes_[2]->access_nodeToParent()[3] = octet::vec4(2, 4, 0, 1);
            nodes_[3]->access_nodeToParent()[3] = octet::vec4(8, -5, 0, 1);
        }*/
    }

    void AddControlPoint(const octet::vec3& point, octet::visual_scene* sc)
    {
        //mesh
        assert(numControlPoints < 4);
        spheres_[numControlPoints]->get_node()->access_nodeToParent()[3] = point.xyz1();
        spheres_[numControlPoints]->set_flags(octet::mesh_instance::flag_enabled);
        numControlPoints++;
        //instance
    }
    
    bool CastRay(octet::vec3 start, octet::vec3 end)
    {
        octet::vec3 direction = (end - start).normalize();
        bool collided = false;

        if (selectedIndex == -1)
        {
            for (unsigned i = 0; i < spheres_.size() && !collided; i++)
            {
                octet::vec3 sphere_center = spheres_[i]->get_node()->access_nodeToParent()[3].xyz();
                float distance = direction.dot(start - sphere_center)*direction.dot(start - sphere_center) - (start - sphere_center).dot((start - sphere_center)) + sphere_size * sphere_size;

                if (distance >= -5.0f) //offset
                {
                    (*spheres_[i]).set_material(selected_mat);
                    selectedIndex = i;
                    collided = true;
                }
            }
        }
        return collided; 
    }

    void ResetSelectedControlPoint()
    {
        if (selectedIndex != -1)
        {
            //currNode_ = NULL;
            (*spheres_[selectedIndex]).set_material(mat);

            selectedIndex = -1;
        }
    }
    
    void MoveControlPoint(octet::vec3 dir)
    {
        if (selectedIndex != -1)
        {
            //(*spheres_[selectedIndex]).set_material(mat);            
            spheres_[selectedIndex]->get_node()->access_nodeToParent().translate(dir * 0.2f);
        }
        Draw();
    }

    void AddControlPoints(octet::visual_scene* sc)
    {
        for (unsigned i = 0; i < 3; i++)
        {
            AddControlPoint(octet::vec3(rand.get(-12, +12), rand.get(-12, 0), 0), sc);
        }
        curves++;
        Draw();
    }

    void Reset()
    {
        if (spheres_.size() != 0)
        {
            numControlPoints = 0;
            for (unsigned i = 0; i < spheres_.size(); i++)
            {
                spheres_[i]->set_flags(0);//the first one is the line
            }
        }
    }
    
    void ChangeVisibility()
    {
        visibility = !visibility;
        if (!visibility)
        {
            for (unsigned i = 0; i < spheres_.size(); i++)
            {
                spheres_[i]->set_flags(0);
            }
        }
        else
        {
            for (unsigned i = 0; i < spheres_.size(); i++)
            {
                spheres_[i]->set_flags(octet::mesh_instance::flag_enabled);
            }
        }
        
    }
    
private:
    int numControlPoints;
    octet::ref<octet::material> mat;
    octet::ref<octet::material> selected_mat;
    octet::ref<octet::mesh> meshy_;

    static octet::mesh* sphereMesh_;
    //octet::dynarray<octet::vec3> controlPoints_;
    //I will use this as a pool object
    octet::dynarray<octet::ref<octet::mesh_instance>> spheres_;
    const float sphere_size = 0.5f; 
    
    //This will be evaluated every time a node is selected;
    //octet::scene_node* currNode_ = NULL;
    int selectedIndex = -1;

    float* vtxP_;
    
    int curves = 0;
    const int drawingPrecision_ = 15;
    bool visibility = true;

    octet::random rand;
     
};

octet::mesh* Bezier::sphereMesh_ = 0;

#endif
