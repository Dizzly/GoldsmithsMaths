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

    void Draw()
    {
        if (nodes_.size() < 4)
        {
            return;
        }

        //Allocating mesh
        int sizenode = nodes_.size();
        int points = curves * drawingPrecision_ + curves;
        int size = points * sizeof(float) * 3;
        meshy_->allocate(size, 0);
        meshy_->set_params(sizeof(float) * 3, 0, points, GL_LINE_STRIP, 0);
        meshy_->clear_attributes();
        meshy_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
        octet::gl_resource::wolock vl(meshy_->get_vertices());
        vtxP_ = (float*)vl.u8();

        octet::vec3 bz_point;
       
        for (unsigned i = 0; i < nodes_.size() - 3; i+=3)
        {
            octet::vec3 p0 = nodes_[i]->access_nodeToParent()[3].xyz();
            octet::vec3 p1 = nodes_[i + 1]->access_nodeToParent()[3].xyz();
            octet::vec3 p2 = nodes_[i + 2]->access_nodeToParent()[3].xyz();
            octet::vec3 p3 = nodes_[i + 3]->access_nodeToParent()[3].xyz();

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
        
        mat = new octet::material(octet::vec4(0, 1, 0, 1));
        selected_mat = new octet::material(octet::vec4(1, 0, 0, 1));
        
        //octet::gl_resource::wolock vl(meshy_->get_vertices());
        //vtxP_ = (float*)vl.u8();
        
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
        octet::mesh_sphere* sphere = new octet::mesh_sphere(octet::vec3(0), sphere_size, 2);
        //node
        octet::scene_node *node = new octet::scene_node();
        node->translate(point);
        nodes_.push_back(node);
        sc->add_child(node);
        //instance
        octet::mesh_instance* mistance = new octet::mesh_instance(node, sphere, mat);
        spheres_.push_back(mistance);        
        sc->add_mesh_instance(mistance);
    }
    
    bool CastRay(octet::vec3 start, octet::vec3 end)
    {
        octet::vec3 direction = (end - start).normalize();
        bool collided = false;

        if (selectedIndex == -1)
        {
            for (unsigned i = 0; i < spheres_.size() && !collided; i++)
            {
                octet::vec3 sphere_center = nodes_[i]->access_nodeToParent()[3].xyz();
                float distance = direction.dot(start - sphere_center)*direction.dot(start - sphere_center) - (start - sphere_center).dot((start - sphere_center)) + sphere_size * sphere_size;

                if (distance >= -5.0f) //offset
                {
                    //(*spheres_[i]).set_material(selected_mat);
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
            //(*spheres_[selectedIndex]).set_material(mat);
            selectedIndex = -1;
        }
    }
    
    void MoveControlPoint(octet::vec3 dir)
    {
        if (selectedIndex != -1)
        {
            //(*spheres_[selectedIndex]).set_material(mat);            
            nodes_[selectedIndex]->access_nodeToParent().translate(dir * 0.2f);
        }
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
        if (nodes_.size() != 0)
        {
            for (unsigned i = 0; i < nodes_.size(); i++)
            {
                nodes_[i]->access_nodeToParent().translate(octet::vec3(100000, 100000, 0));//the first one is the line
            }
            nodes_.reset();
            spheres_.reset();
        }
    }
    
    void ChangeVisibility()
    {
        visibility = !visibility;
        if (!visibility)
        {
            for (unsigned i = 0; i < nodes_.size(); i++)
            {
                octet::vec3 currentpos = nodes_[i]->access_nodeToParent()[3].xyz();
                nodes_[i]->access_nodeToParent().translate(octet::vec3(0, 0, -100000));//the first one is the line
            }
        }
        else
        {
            for (unsigned i = 0; i < nodes_.size(); i++)
            {
                octet::vec3 currentpos = nodes_[i]->access_nodeToParent()[3].xyz();
                nodes_[i]->access_nodeToParent().translate(octet::vec3(0, 0, -currentpos.z()));//the first one is the line
            }
        }
        
    }
    
private:
    
    octet::material* mat;
    octet::material* selected_mat;
    octet::ref<octet::mesh> meshy_;
    //octet::dynarray<octet::vec3> controlPoints_;
    octet::dynarray<octet::mesh_instance*> spheres_;
    const float sphere_size = 0.5f; 
    octet::dynarray<octet::scene_node*> nodes_;
    
    //This will be evaluated every time a node is selected;
    octet::scene_node* currNode_ = NULL;
    int selectedIndex = -1;

    float* vtxP_;
    
    int curves = 0;
    const int drawingPrecision_ = 15;
    bool visibility = true;

    octet::random rand;
     
};



#endif
