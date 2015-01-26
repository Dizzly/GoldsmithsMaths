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
       std::vector<Equation::EquatFunc*> equations_;

        TwBar* bar_;
        int value_;

        typedef enum { COMPLEX, SPIRO1, SPIRO2 } FunctionsType;
        FunctionsType ftype = COMPLEX;

        int functionArgumentSizes[3];
        int displayFunctionArgumentSize;
        

        struct SpiroState
        {
            float t;
            vec4 color;
            float thickness;
            FunctionsType functionIndex;
            std::vector<float> params;

            const SpiroState& operator =(const SpiroState& sp)
            {
                t = sp.t;
                color = sp.color;
                thickness = sp.thickness;
                functionIndex = sp.functionIndex;
                params.clear();
                params.insert(params.begin(), sp.params.begin(), sp.params.end());
                return *this;
            }
            bool operator ==(const SpiroState& sp)
            {
                return (
                    t == sp.t &&
                    color.x()== sp.color.x() &&
                    color.y()== sp.color.y() &&
                    color.z()== sp.color.z() &&
                    color.w() ==sp.color.w() &&
                    thickness == sp.thickness&&
                    functionIndex == sp.functionIndex&&
                    params.size() == sp.params.size()&&
                    ParamtersAreTheSame(sp.params)
                    );
            }

        private:
            bool ParamtersAreTheSame(const std::vector<float>& p)
            {
                for (int i = 0;i< params.size(); ++i)
                {
                    if (params[i] != p[i])
                    {
                        return false;
                    }
                }
                return true;
            }
        };

        SpiroState curr;
        SpiroState old;

        ParametricCurve curve_;

        Bezier bz;
        octet::random rand;

        int resolution;

        //for mouse movement
        float prev_mmX;
        float prev_mmY;

        int x_res;
        int y_res;

        int currentEquation_;
        int currentSpiroMesh_;
        bool shouldPushMesh_;
        bool shouldResetSpiro_;

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
                    curr.params = { 80, 1, 80, 1, 3, 3 };
                    curve_.SetParameters(Equation::InputParameters(curr.params.data(), 6));
                    curve_.Draw(curveMeshes_[currentSpiroMesh_], curr.t);
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
                    
                    
                    TwDefine(" TweakBar visible=false ");

                    mode = Mode::BezierControl;
                }
                else
                {
                    //Resetting Bezier
                    bz.Reset();
                    bezMeshInst->set_flags(0);
                    // Enabling Spiro
                    for (int i = 0; i < meshInst_.size(); ++i)
                    {
                        meshInst_[i]->set_flags(mesh_instance::flag_enabled);
                    }
                    
                    TwDefine(" TweakBar visible=true ");  // mybar is hidden

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
                octet::vec3 newPos(rand.get(-12, 12), rand.get(-12, 12), 0);
                bz.AddControlPoint(newPos,app_scene);
                bz.Draw();
            }

            if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
            {
                bz.ResetSelectedControlPoint();
                TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
            }
        }
        
        void Regen()
        {
            if (!(curr==old) && mode==Mode::Spiro)
            {
                curve_.SetColor(MakeColor(curr.color));
                curve_.SetEquation(equations_[curr.functionIndex]);
                curve_.SetParameters(Equation::InputParameters(curr.params.data(),curr.params.size()));
                curve_.SetThickness(curr.thickness);
                curve_.Draw(curveMeshes_[currentSpiroMesh_], curr.t);
                old = curr;
                displayFunctionArgumentSize = functionArgumentSizes[old.functionIndex];
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
            curve_.SetEquation(equations_[curr.functionIndex]);
            curve_.SetParameters(Equation::InputParameters(curr.params.data(), curr.params.size()));
            curve_.SetThickness(curr.thickness);
            curve_.SetColor(MakeColor(curr.color));
            currentSpiroMesh_ = 0;
        }

    public:
        /// this is called when we construct the class before everything is initialised.
        Spirographs(int argc, char **argv) : app(argc, argv) {
            curr.t = 1.0f;
            curr.color = vec4(1, 1, 1, 1);
            shouldPushMesh_ = false;
            shouldResetSpiro_ = false;
            currentSpiroMesh_ = 0;
            curr.thickness = 3;
            equations_.push_back(PrettyFunction);
            equations_.push_back(Hypotrochoid);
            equations_.push_back(Epitrochoid);
            functionArgumentSizes[0] = 6;
            functionArgumentSizes[1] = 3;
            functionArgumentSizes[2] = 3;
            curr.functionIndex=COMPLEX;
            old = curr;
        }

        /// this is called once OpenGL is initialized
        void app_init() {
            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();
            app_scene->get_camera_instance(0)->set_far_plane(100000000);
            app_scene->get_camera_instance(0)->get_node()->access_nodeToParent()[3].z() = 6.5f;
            GetScreenSize();

            param_shader* shader = new param_shader("shaders/default.vs",
                "src/examples/Spirograph/custom_solid.fs");
            mat = new material(vec4(1,1,1,1),shader);

            bezMat = new material(vec4(0, 0, 0, 1));

            bezMesh = new mesh(); 

            curveMeshes_.push_back(new mesh());

            mode = Mode::Spiro;

            curve_.SetEquation(equations_[curr.functionIndex], 6, 2);


            curr.params= { 1,80,1,80,3,3};
            curve_.SetParameters(Equation::InputParameters(curr.params.data(), 6));

            curve_.SetMaxResolution(1000);
            curve_.SetThickness(curr.thickness);

            curve_.Draw(curveMeshes_[currentSpiroMesh_], curr.t);

            //TWBAr initialisation
            TwInit(TW_OPENGL, NULL);
            TwWindowSize(768, 768 - 40);//minus 30 because "I dont know why"
                       

            bar_ = TwNewBar("TweakBar");

            TwEnumVal functionsEV[3];
            //FunctionEnumInitialisation
            // Defining function enum type
            functionsEV[0] = { COMPLEX, "Complex" };
            functionsEV[1] = { SPIRO1, "Spiro 1" };
            functionsEV[2] = { SPIRO2, "Spiro 2" };

            TwType eNum = TwDefineEnum("FunctionType", functionsEV, 3);
            // Adding season to bar
            TwAddVarRW(bar_, "Function",eNum, &curr.functionIndex, NULL);
            TwAddVarRO(bar_, "Number of arguments for function", TW_TYPE_INT32, &displayFunctionArgumentSize, "Help='The number of arguments the chosen function takes, others are ignored'");
            TwAddVarRW(bar_, "Value1", TW_TYPE_FLOAT, &curr.params[0], " label='First Param' Min=0.01f ");
            TwAddVarRW(bar_, "Value2", TW_TYPE_FLOAT, &curr.params[1], " label='Second Param'Min=0.01f ");
            TwAddVarRW(bar_, "Value3", TW_TYPE_FLOAT, &curr.params[2], " label='Third Param' Min=0.01f");
            TwAddVarRW(bar_, "Value4", TW_TYPE_FLOAT, &curr.params[3], " label='Fourth Param' Min=0.01f");
            TwAddVarRW(bar_, "Value5", TW_TYPE_FLOAT, &curr.params[4], " label='Fifth Param' Min=0.01f");
            TwAddVarRW(bar_, "Value6", TW_TYPE_FLOAT, &curr.params[5], " label='Sixth Param' Min=0.01f");
            TwAddVarRW(bar_, "T value", TW_TYPE_FLOAT, &curr.t, "Step=0.001f Min=0.0f Max=100f");

            TwAddVarRW(bar_, "Line Thickness", TW_TYPE_FLOAT, &curr.thickness, "");
            TwAddVarRW(bar_, "Line Color", TW_TYPE_COLOR3F, &curr.color, " label='LineColor' ");
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
