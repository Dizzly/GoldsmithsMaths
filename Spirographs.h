////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "Equation.h"
#include "ParametricCurve.h"


namespace octet {

    /// Scene drawing spirograph
    class Spirographs : public app {

    private:
        
        enum Mode{Spiro, BezierControl};

        Mode mode;

        // scene for drawing box
        ref<visual_scene> app_scene;

        ref<material> mat;
        ref<mesh> curveMesh;

        TwBar* bar_;
        int value_;
        
        bool hasChanged_;

        //Adding camera control
        mouse_ball camera;

        ParametricCurve curve;
        Bezier bz;

        float t;
        float oldT;
        int resolution;

        //for mouse movement
        float prev_mmX;
        float prev_mmY;

        int x_res;
        int y_res;

        float color[4];

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

            if (is_key_down(key::key_left))
            {
                float params[10] = { 80,1,80,1,3,3 };
                curve.SetParameters(Equation::InputParameters(params, 6));
                curve.Draw(curveMesh, t);
                t = 0;
            }
            if (is_key_down(key::key_left))
            {
                if (mode == Mode::Spiro)
                {
                    float params[10] = { 80, 1, 80, 1, 3, 3 };
                    curve.SetParameters(Equation::InputParameters(params, 6));
                    curve.Draw(curveMesh, 8);
                    t = 0;
                }
            }
            if (is_key_going_down(key::key_space))
            {
                //Switching between the two of them sorry this is bad
                if (mode == Mode::Spiro)
                {
                    bz.Init(app_scene, curveMesh);
                    bz.Draw();
                    mode = Mode::BezierControl;
                }
                else
                {
                    bz.Reset();
                    float params[10] = { 80, 1, 80, 1, 3, 3 };
                    curve.SetParameters(Equation::InputParameters(params, 6));
                    curve.Draw(curveMesh, 8);
                    t = 0;
                    mode = Mode::Spiro;
                }
                
            }
        }

        void MouseInputControl()
        {
            int mX = 0, mY = 0;
            get_mouse_pos(mX, mY);
            TwMouseMotion(mX, mY);

            if (is_key_down(key_lmb))
            {
                TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
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
                bz.AddControlPoints(app_scene);
            }

            if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
            {
                bz.ResetSelectedControlPoint();
                TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
            }
        }

        void Regen()
        {
            if (t != oldT)
            {
                curve.Draw(curveMesh, t);
                oldT = t;
            }
            curve.Draw(curveMesh, t);           
        }

    public:
        /// this is called when we construct the class before everything is initialised.
        Spirographs(int argc, char **argv) : app(argc, argv) {
            t = 0;
            oldT = t;
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
            color[3] = 0;
            hasChanged_ = false;
        }

        /// this is called once OpenGL is initialized
        void app_init() {
            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();
            app_scene->get_camera_instance(0)->set_far_plane(101);

            GetScreenSize();

            mat = new material(vec4(1, 0, 0, 1));
            curveMesh = new mesh();

            mode = Mode::Spiro;

            curve.SetEquation(PrettyFunction, 6, 2);

            float params[10] = { 80,1,1,80,3,3};
            curve.SetParameters(Equation::InputParameters(params, 6));

            curve.SetMaxResolution(500);
            curve.SetThickness(1);

            curve.Draw(curveMesh, t);

            TwInit(TW_OPENGL, NULL);
            TwWindowSize(768, 768 - 60);//minus 30 because "I dont know why"

            bar_ = TwNewBar("TweakBar");

            TwAddVarRW(bar_, "T value", TW_TYPE_FLOAT, &t, "Step=0.01f Min=0.0f");
            TwAddVarRW(bar_, "bgColor", TW_TYPE_COLOR3F, &color, " label='Background color' ");

            curve.Draw(curveMesh, 8);

            app_scene->add_mesh_instance(new mesh_instance(new scene_node(), curveMesh, mat));

            //initializing the camera
            camera.init(this, 1, 100);
         
        }

        /// this is called to draw the world
        void draw_world(int x, int y, int w, int h)
        {
            Regen();

            KeyboardInputControl();
            HackyKeyboardTranslation();
            MouseInputControl();

            int vx = 0, vy = 0;
            get_viewport_size(vx, vy);
            app_scene->begin_render(vx, vy);

            mat->set_diffuse(vec4(color[0], color[1], color[2], color[3]));

            //updating camera
            camera.update(app_scene->get_camera_instance(0)->get_node()->access_nodeToParent());

            // update matrices. assume 30 fps.
            app_scene->update(1.0f / 30);

            // draw the scene
            app_scene->render((float)vx / vy);
            // tumble the box  (there is only one mesh instance)
            TwDraw();

        }
    };
}
