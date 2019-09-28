#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
///////////////////////////////////////////////////////////////////////////////
class Shader
{
    GLuint sID;
    
public:
    GLuint id() const { return sID; }
    //-----------------------------------------------------------------------------
    Shader(const char * vert, const char * frag)
    {
        // 1. CREATE SHADER PROGRAM
        sID = glCreateProgram();
        GLuint vID = glCreateShader( GL_VERTEX_SHADER );
        GLuint fID = glCreateShader( GL_FRAGMENT_SHADER );
        // 2. LOAD SHADER SOURCE CODE
        glShaderSource( vID, 1, &vert, NULL ); // Last arg specifies length of source string
        glShaderSource( fID, 1, &frag, NULL );
        // 3. COMPILE
        glCompileShader( vID );
        glCompileShader( fID );
        // 4. CHECK FOR COMPILE ERRORS
        compilerCheck( vID );
        compilerCheck( fID );
        // 5. ATTACH SHADERS TO PROGRAM
        glAttachShader( sID,vID );
        glAttachShader( sID,fID );
        // 6. LINK PROGRAM
        glLinkProgram( sID );
        // 7. CHECK FOR LINKING ERRORS
        linkCheck( sID );
        // 8. USE PROGRAM
        glUseProgram( sID );
    }
    //-----------------------------------------------------------------------------
    void bind(){ glUseProgram( sID ); }
    //-----------------------------------------------------------------------------
    void unbind() { glUseProgram( 0 ); }
    //-----------------------------------------------------------------------------
    void compilerCheck(GLuint ID)
    {
        GLint comp;
        glGetShaderiv(ID, GL_COMPILE_STATUS, &comp);
        
        if( comp == GL_FALSE )
        {
            cout << "Shader Compilation FAILED" << endl;
            GLchar messages[256];
            glGetShaderInfoLog(ID, sizeof(messages),0,&messages[0]);
            cout << messages;
        }
    }
    //-----------------------------------------------------------------------------
    void linkCheck(GLuint ID)
    {
        GLint linkStatus, validateStatus;
        glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);
        
        if(linkStatus == GL_FALSE )
        {
            cout << "Shader Linking FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages),0,&messages[0]);
            cout << messages;
        }
        
        glValidateProgram(ID);
        glGetProgramiv(ID, GL_VALIDATE_STATUS, &validateStatus);
        
        cout << "Link: " << linkStatus << "  Validate: " << validateStatus << endl;
        if(linkStatus == GL_FALSE )
        {
            cout << "Shader Validation FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages),0,&messages[0]);
            cout << messages;
        }
    }
    //-----------------------------------------------------------------------------
};
///////////////////////////////////////////////////////////////////////////////
