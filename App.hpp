#include "window.hpp"
///////////////////////////////////////////////////////////////////////////////
struct App
{
    Window mWindow;
    Window& window() { return mWindow; }
    //-----------------------------------------------------------------------------
    App( int w = 640, int h=580 )
    {
        if( !glfwInit() ) exit( EXIT_FAILURE );
        mWindow.create( this, w ,h );
        
        glewExperimental = true;
        if ( glewInit() != GLEW_OK)  exit( EXIT_FAILURE );
        
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LESS );
        
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        
        glLineWidth( 3 );               //<-- Thicken lines so we can see 'em clearly
    }
    //-----------------------------------------------------------------------------
    void start()
    {
        while ( !mWindow.shouldClose() )
        {
            mWindow.setViewport();
            glClearColor( (GLclampf)0, (GLclampf)0, (GLclampf)0, (GLclampf)1 );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            onDraw();
            mWindow.swapBuffers();      //<-- SWAP BUFFERS
            glfwPollEvents();           //<-- LISTEN FOR WINDOW EVENTS
        }
    }
    //-----------------------------------------------------------------------------
    ~App(){ glfwTerminate(); }
    //-----------------------------------------------------------------------------
    virtual void onDraw() {}
    //-----------------------------------------------------------------------------
    virtual void onMouseMove( int x, int y ){}
    //-----------------------------------------------------------------------------
    virtual void onMouseDown( int button, int action ){}
    //-----------------------------------------------------------------------------
    virtual void onKeyDown( int key, int action ) {}
    //-----------------------------------------------------------------------------
};
///////////////////////////////////////////////////////////////////////////////
