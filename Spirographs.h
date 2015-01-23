////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "Equation.h"
#include "ParametricCurve.h"


namespace octet {


    void TW_CALL MakeTrue(void *v)
    {
        bool* bp = (bool*)v;
        *bp = true;
    }
    /// Scene drawing spirograph
    class Spirographs : public app {

    private:
        
        enum Mode{Spiro, BezierControl};

        Mode mode;

        // scene for drawing box
        ref<visual_scene> app_scene;

        ref<material> mat;
        ref<material> bezMat;
        ref<mesh> bezMesh;
        ref<mesh_instance> bezMeshInst;

        std::vector<float> params;

       std::vector<ref<mesh>> curveMeshes_;
       std::vector < mesh_instance*> meshInst_;
       std::vector<Equation> equations_;

        TwBar* bar_;
        int value_;
        
        bool hasChanged_;

        ParametricCurve curve_;

        Bezier bz;

        float t;
        float oldT;
        int resolution;
        float thickness_;
        float oldThickness_;

        //for mouse movement
        float prev_mmX;
        float prev_mmY;

        int x_res;
        int y_res;

        int currentEquation_;
        int currentSpiroMesh_;
        bool shouldPushMesh_;
        bool shouldResetSpiro_;

        vec4 color;
        vec4 oldColor;

        bool AreTheSame(vec4 v1, vec4 v2)
        {
            return(
                v1.x() == v2.x() &&
                v1.y() == v2.y() &&
                v1.z() == v2.z() &&
                v1.w() == v2.w()
                );
        }

        uint32_t MakeColor(vec4 col)
        {
            int r = col.x() * 255;
            int g = col.y() * 255;
            int b = col.z() * 255;
            int a = col.w() * 255;

            uint32_t t = g << 8;
            uint32_t v = r + g << 8;

            return uint32_t(r + (g << 8) + (b << 16) +(a<<24));
        }

        void GetScreenSize()
        {
            bool fResult;
            RECT rectWorkArea;

            //Chuck: checking for screen size
            fResult = SystemParametersInfo(SPI_GETWORKAREA, 
                0,                // Not used
                &rectWorkArea,    
                0);

            x_res = (int)rectWorkArea.bottom;
            y_res = (int)rectWorkArea.right;

            /*if (fResult)
            {
            printf("Windows size: %u %u %u %u \n", rectWorkArea.left, rectWorkArea.bottom, rectWorkArea.right, rectWorkArea.top);
            }*/
        }

        void HackyKeyboardTranslation()
        {
            if (is_key_going_down('1'))
            {
                TwKeyPressed('1', 0);
            }
            if (is_key_going_down('2'))
            {
                TwKeyPressed('2', 0);
            }
            if (is_key_going_down('3'))
            {
                TwKeyPressed('3', 0);
            }
            if (is_key_going_down('4'))
            {
                TwKeyPressed('4', 0);
            }
            if (is_key_going_down('5'))
            {
                TwKeyPressed('5', 0);
            }
            if (is_key_going_down('6'))
            {
                TwKeyPressed('6', 0);
            }
            if (is_key_going_down('7'))
            {
                TwKeyPressed('7', 0);
            }
            if (is_key_going_down('8'))
            {
                TwKeyPressed('8', 0);
            }
            if (is_key_going_down('9'))
            {
                TwKeyPressed('9', 0);
            }
            if (is_key_going_down('0'))
            {
                TwKeyPressed('0', 0);
            }
            //for some reason 190 is the key code for '.'
            //46 is the ascii code for '.'
            if (is_key_going_down(190))
            {
                TwKeyPressed(46, 0);
            }
            //control values using ascii found here
            //http://www.asciitable.com/
            if (is_key_going_down(key_backspace))
            {
                TwKeyPressed(8, 0);
            }
            
        }

        void KeyboardInputControl()
        {
            if (is_key_down(key::key_esc))
            {
                exit(1);
            }
            float speed = 1;
            if (is_key_down(key::key_shift))
            {
                speed = 0.08;
            }
            if (is_key_down('W'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, speed, 0);
            }
            if (is_key_down('S'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -speed, 0);
            }
            if (is_key_down('E'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -speed);
            }
            if (is_key_down('Q'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, speed);
            }
            if (is_key_down('A'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(-speed, 0, 0);
            }
            if (is_key_down('D'))
            {
                app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(speed, 0, 0);
            }
            if (is_key_going_down('V'))
            {
                if (mode == Mode::BezierControl)
                {
                    bz.ChangeVisibility();
                }
            }
            if (is_key_going_down(key::key_left))
            {
                if (mode == Mode::Spiro)
                {
                    params = { 80, 1, 80, 1, 3, 3 };
                    curve_.SetParameters(Equation::InputParameters(params.data(), 6));
                    curve_.Draw(curveMeshes_[currentSpiroMesh_], t);
                }
            }
            if (is_key_going_down(key::key_space))
            {
                //Switching between the two of them sorry this is bad
                if (mode == Mode::Spiro)
                {
                    //Disable drawing of the spiro meshes
                    for (int i = 0; i < meshInst_.size(); ++i)
                    {
                        meshInst_[i]->set_flags(0);
                    }
                    //Enabling Bezier curves
                    bezMeshInst->set_flags(mesh_instance::flag_enabled);
                    bz.Init(app_scene, bezMesh);
                    bz.Draw();
                    
                    mode = Mode::BezierControl;
                }
                else
                {
                    bz.Reset();
                    for (int i = 0; i < meshInst_.size(); ++i)
                    {
                        meshInst_[i]->set_flags(mesh_instance::flag_enabled);
                    }
                    bezMeshInst->set_flags(0);
                    mode = Mode::Spiro;
                }                
            }
        }

        void MouseInputControl()
        {
            int mX = 0, mY = 0;
            get_mouse_pos(mX, mY);
            TwMouseMotion(mX, mY);
            if (is_key_going_down(key_lmb))
            {
                TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
            }
            if (is_key_down(key_lmb))
            {
                
                //printf("%i,%i\n", mX, mY);
                if (mode == Mode::BezierControl)
                {
                    float mmX = (mX - 768.0f / 2) / (768.0f / 2);
                    float mmY = -(mY - 768.0f / 2) / (768.0f / 2);

                    printf("%f %f\n",mmX, mmY);

                    octet::ray raycast = app_scene->get_camera_instance(0)->get_ray(mmX, mmY);
                    
                    /*
                    float xsd = raycast.get_start().x();
                    float ysd = raycast.get_start().y();

                    float xed = raycast.get_end().x();
                    float yed = raycast.get_end().y();

                    scene_node* s = new scene_node();
                    app_scene->add_scene_node(s);
                    s->access_nodeToParent()[3] = vec4(xed, yed, 0, 1);

                    app_scene->add_mesh_instance(new mesh_instance(s, new mesh_box(vec3(1, 1, 1)), mat));*/
                                        
                    bz.CastRay(raycast.get_start(), raycast.get_end());
                    
                    if (prev_mmX != mmX || prev_mmY != mmY)
                    {
                        vec3 dir(mmX - prev_mmX, mmY - prev_mmY, 0);
                        dir = dir.normalize();
                        bz.MoveControlPoint(dir);
                        bz.Draw();
                    }

                    prev_mmX = mmX;
                    prev_mmY = mmY;

                }
            }
            else if(is_key_going_down(key_rmb))
            {
                //bz.AddControlPoints(app_scene);
            }

            if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
            {
                bz.ResetSelectedControlPoint();
                TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
            }
        }
        
        void Regen()
        {
            if ((t != oldT || !AreTheSame(color, oldColor)||thickness_!=oldThickness_)&&mode==Mode::Spiro)
            {
                curve_.SetColor(MakeColor(color));
                curve_.SetParameters(Equation::InputParameters(params.data(), 6));
                curve_.SetThickness(thickness_);
                curve_.Draw(curveMeshes_[currentSpiroMesh_], t);
                oldT = t;
                oldThickness_ = thickness_;
            }           
        }

        void PushSpiroMesh()
        {
            currentSpiroMesh_++;
            if (currentSpiroMesh_ >= meshInst_.size())
            {
                curveMeshes_.push_back(new mesh());
                meshInst_.push_back(new mesh_instance(new scene_node(), curveMeshes_.back(), mat, 0));
                app_scene->add_mesh_instance(meshInst_.back());
            }
            else
            {
                meshInst_[currentSpiroMesh_]->set_flags(mesh_instance::flag_enabled);
            }

        }

        void ResetSpiroMesh()
        {
            for (int i = curveMeshes_.size()-1; i > 0; --i)
            {
                meshInst_[i]->set_flags(0);
            }
            params = { 80, 1, 1, 80, 3, 3 };
            curve_.SetParameters(Equation::InputParameters(params.data(), 6));
            curve_.SetThickness(thickness_);
            curve_.SetColor(MakeColor(color));
            currentSpiroMesh_ = 0;
        }

    public:
        /// this is called when we construct the class before everything is initialised.
        Spirographs(int argc, char **argv) : app(argc, argv), color(1, 1, 1, 1) {
            t = 1;
            oldT = t;
            oldColor = color;
            hasChanged_ = false;
            shouldPushMesh_ = false;
            shouldResetSpiro_ = false;
            currentSpiroMesh_ = 0;
            thickness_ = 3;
            oldThickness_ = thickness_;

        }

        /// this is called once OpenGL is initialized
        void app_init() {
            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();
            app_scene->get_camera_instance(0)->set_far_plane(101);

            GetScreenSize();

            param_shader* shader = new param_shader("shaders/default.vs",
                "src/examples/Spirograph/custom_solid.fs");
            mat = new material(vec4(1,1,1,1),shader);

            bezMat = new material(vec4(0, 0, 0, 1));

            bezMesh = new mesh(); 

            curveMeshes_.push_back(new mesh());

            mode = Mode::Spiro;

            //Spiro initialisation
            curve_.SetEquation(PrettyFunction, 6, 2);

            params= { 80,1,1,80,3,3};
            curve_.SetParameters(Equation::InputParameters(params.data(), 6));

            curve_.SetMaxResolution(1000);
            curve_.SetThickness(thickness_);

            curve_.Draw(curveMeshes_[currentSpiroMesh_], t);

            //TWBAr initialisation
            TwInit(TW_OPENGL, NULL);
            TwWindowSize(768, 768 - 76);//minus 30 because "I dont know why"

            bar_ = TwNewBar("TweakBar");

            TwAddVarRW(bar_, "Value A", TW_TYPE_FLOAT, &params[0], "");
            TwAddVarRW(bar_, "Value B", TW_TYPE_FLOAT, &params[1], "");
            TwAddVarRW(bar_, "Value C", TW_TYPE_FLOAT, &params[2], "");
            TwAddVarRW(bar_, "Value D", TW_TYPE_FLOAT, &params[3], "");
            TwAddVarRW(bar_, "Value E", TW_TYPE_FLOAT, &params[4], "");
            TwAddVarRW(bar_, "Value F", TW_TYPE_FLOAT, &params[5], "");
            TwAddVarRW(bar_, "T value", TW_TYPE_FLOAT, &t, "Step=0.001f Min=0.0f Max=1.0f");
            TwAddVarRW(bar_, "Line Thickness", TW_TYPE_FLOAT, &thickness_, "");
            TwAddVarRW(bar_, "Line Color", TW_TYPE_COLOR3F, &color, " label='LineColor' ");
            TwAddButton(bar_, "New Spiro", MakeTrue,&shouldPushMesh_,"");
            TwAddButton(bar_, "Clear Spiros", MakeTrue, &shouldResetSpiro_, "");
            
            //Bezier initialization
            meshInst_.push_back(new mesh_instance(new scene_node(), curveMeshes_[currentSpiroMesh_], mat));
            bezMeshInst = new mesh_instance(new scene_node(), bezMesh, bezMat);

            app_scene->add_scene_node(bezMeshInst->get_scene_node());
            app_scene->add_mesh_instance(bezMeshInst);

            app_scene->add_mesh_instance(meshInst_[currentSpiroMesh_]);
         
        }

        /// this is called to draw the world
        void draw_world(int x, int y, int w, int h)
        {
            Regen();
            if (shouldPushMesh_)
            {
                PushSpiroMesh();
                shouldPushMesh_ = false;
            }
            if(shouldResetSpiro_)
            {
                ResetSpiroMesh();
                shouldResetSpiro_ = false;
            }

            KeyboardInputControl();
            HackyKeyboardTranslation();
            MouseInputControl();

            int vx = 0, vy = 0;
            get_viewport_size(vx, vy);
            app_scene->begin_render(vx, vy);

            // update matrices. assume 30 fps.
            app_scene->update(1.0f / 30);

            // draw the scene
            app_scene->render((float)vx / vy);
            // tumble the box  (there is only one mesh instance)
            TwDraw();

        }
    };
}
