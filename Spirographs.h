////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "AntTweakBar\AntTweakBar.h"
#include "Equation.h"
#include "ParametricCurve.h"

namespace octet {

    /// Scene drawing spirograph
    class Spirographs : public app {

    private:
        // scene for drawing box
        ref<visual_scene> app_scene;

        ref<material> mat;
        ref<mesh> curveMesh;

        TwBar* bar_;
        int value_;

        //Adding camera control
        mouse_ball camera;

        ParametricCurve curve;

        float t = 0.0001f;
        float t_increase = 0.04f;
        int resolution;

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
            if (is_key_down(key::key_up))
            {
                GenerateNewStep();
            }
            if (is_key_down(key::key_left))
            {
                float params[10] = { 80,1,80,1,3,3 };
                curve.SetParameters(Equation::InputParameters(params, 6));
                curve.Draw(curveMesh, 8);
                t = 0;
            }
        }

        void GenerateNewStep()
        {
            t += t_increase;
            curve.Draw(curveMesh, t);
            
            
        }

        

    public:
        /// this is called when we construct the class before everything is initialised.
        Spirographs(int argc, char **argv) : app(argc, argv) {
        }

        /// this is called once OpenGL is initialized
        void app_init() {
            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();
            app_scene->get_camera_instance(0)->set_far_plane(1000000);

            mat = new material(vec4(1, 0, 0, 1));
            curveMesh = new mesh();

            curve.SetEquation(PrettyFunction, 6, 2);

            float params[10] = { 80,1,1,80,3,3};
            curve.SetParameters(Equation::InputParameters(params, 6));

            curve.SetMaxResolution(500);
            curve.SetThickness(1);
            curve.Draw(curveMesh, 8);
         
            app_scene->add_mesh_instance(new mesh_instance(new scene_node(), curveMesh, mat));

            //initializing the camera
            camera.init(this, 1, 100);
         
        }

        /// this is called to draw the world
        void draw_world(int x, int y, int w, int h) {
           
            KeyboardInputControl();
            int vx = 0, vy = 0;
            get_viewport_size(vx, vy);
            app_scene->begin_render(vx, vy);

            //updating camera
            camera.update(app_scene->get_camera_instance(0)->get_node()->access_nodeToParent());

            // update matrices. assume 30 fps.
            app_scene->update(1.0f / 30);

            // draw the scene
            app_scene->render((float)vx / vy);
            // tumble the box  (there is only one mesh instance)
        }
    };
}
