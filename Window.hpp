#include <stdio.h>
#include <stdlib.h>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//                 Some Callbacks to be implemented later                    //
///////////////////////////////////////////////////////////////////////////////
struct Interface
{
    static void * app; // <-- an unknown application to be defined later
    
    template<class APPLICATION>
    static void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
    { ((APPLICATION*)(app))->onKeyDown(key,action); }
    
    template<class APPLICATION>
    static void OnMouseMove(GLFWwindow* window, double x, double y)
    { ((APPLICATION*)(app))->onMouseMove( (int)x, (int) y); }
    
    template<class APPLICATION>
    static void OnMouseDown(GLFWwindow* window, int button, int action, int mods)
    { ((APPLICATION*)(app))->onMouseDown(button,action); }
};
///////////////////////////////////////////////////////////////////////////////
void * Interface::app;
///////////////////////////////////////////////////////////////////////////////
class Window
{
public:
    GLFWwindow * window;
    Interface interface;
    
    int mWidth, mHeight;
    
    int width()  { return mWidth; }
    int height() { return mHeight; }
    float ratio() { return (float)mWidth/mHeight;}
    
    Window() {}
    
    template<class APPLICATION>
    //-----------------------------------------------------------------------------
    void create( APPLICATION * app, int w, int h, const char * name="10% Project" )
    {
        interface.app = app;
        mWidth = w; mHeight = h;
        
        window = glfwCreateWindow( w, h, name, NULL, NULL );
        if (!window)
        {
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent( window );
        glfwSwapInterval(1); //<-- force interval (not guaranteed to work with all graphics drivers)
        
        //register callbacks for keyboard and mouse
        glfwSetKeyCallback( window, Interface::OnKeyDown<APPLICATION> );
        glfwSetCursorPosCallback( window, Interface::OnMouseMove<APPLICATION> );
        glfwSetMouseButtonCallback( window, Interface::OnMouseDown<APPLICATION> );
        
    }
    //-----------------------------------------------------------------------------
    void setViewport()
    {
        glfwGetFramebufferSize( window, &mWidth, &mHeight );
        glViewport( 0, 0, mWidth, mHeight );
    }
    //-----------------------------------------------------------------------------
    bool shouldClose() { return glfwWindowShouldClose( window ) == 1; }
    //-----------------------------------------------------------------------------
    void swapBuffers() { glfwSwapBuffers( window ); }
    //-----------------------------------------------------------------------------
    void destroy() { glfwDestroyWindow( window ); }
    //-----------------------------------------------------------------------------
    ~Window() { destroy(); }
    //-----------------------------------------------------------------------------
};
///////////////////////////////////////////////////////////////////////////////
