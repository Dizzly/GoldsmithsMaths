////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  
#ifndef PI
    #define PI 3.1415926535
#endif
#ifndef PI_2
#define PI_2 6.28318530718
#endif
  /// Scene containing a box with octet.
  class Spirographs : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
    
    ref<material> mat;
    ref<mesh_box> boxMesh;
  public:
    /// this is called when we construct the class before everything is initialised.
    Spirographs(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      mat = new material(vec4(1, 0, 0, 1));
      boxMesh = new mesh_box(vec3(0.05));

      float R = 20;
      float r = 1;
      float d = 4;
      float t = 0.5;

      int iterations = 2000;
      int loopMax = iterations *t;
      float angleIncre = PI_2 / (float)iterations;
      for (int i = 0; i < loopMax; ++i)
      {
          float x = 0, y = 0;
          
          x = (R - r)*cos(angleIncre*i) + d*cos(((R - r) / r)*(angleIncre*i));
          y = (R - r)*sin(angleIncre*i) - d*sin(((R - r) / r)*(angleIncre*i));
          scene_node* p = new scene_node();
          app_scene->add_child(p);
          p->access_nodeToParent()[3] = vec4(x, y, 10, 1);
          app_scene->add_mesh_instance(new mesh_instance(p, boxMesh, mat));
      
      }
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
        if (is_key_down('W'))
        {
            app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 1, 0);
        }
        if (is_key_down('S'))
        {
            app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -1, 0);
        }
        if (is_key_down('E'))
        {
            app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -1);
        }
        if (is_key_down('Q'))
        {
            app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, 1);
        }
        

      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the box  (there is only one mesh instance)
    }
  };
}
