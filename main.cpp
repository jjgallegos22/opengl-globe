#include "App.hpp"
#include "Shader.hpp"
#include <math.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLSL(version, A) "#version " #version "\n" #A
#define DISPLAY_WIDTH   640
#define DISPLAY_HEIGHT  580
#define DISPLAY_RATIO   static_cast<float>( DISPLAY_WIDTH ) / DISPLAY_HEIGHT
#define PI                     3.141592653589
#define SPHERE_RADIUS          0.8f
#define SPHERE_STACKS          40
#define SPHERE_SLICES          40
#define SPHERE_TOTAL_VERTICES  SPHERE_STACKS * ( SPHERE_SLICES + 1 ) * 2
///////////////////////////////////////////////////////////////////////////////
const char * vert =  GLSL(120,
                          attribute vec3 position;
                          attribute vec3 normal;
                          uniform mat4 model;
                          uniform mat4 view;
                          uniform mat4 projection;
                          attribute vec2 textureCoordinate;
                          varying vec2 texCoord;
                          
                          void main(){
                              texCoord = textureCoordinate;
                              gl_Position = projection * view * model * vec4( position, 1.0 );
                          }
                          );

const char * frag =  GLSL(120,
                          varying vec2 texCoord;
                          uniform sampler2D myTexture;
                          void main(){
                              //gl_FragColor = vec4 (0.0, 1.0, 0.0, 1.0);
                              gl_FragColor = vec4(texture2D(myTexture, texCoord).rgb, 1.0);
                          }
                          );
///////////////////////////////////////////////////////////////////////////////
struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec2 texture;
};

struct Bitmap{
    
    
    int width;
    int height;
    short BitsPerPixel;
    std::vector<unsigned char> pixels;
    
    Bitmap(const char* FilePath) : width(0), height(0), BitsPerPixel(0){
        load(FilePath);
    }
    
    //adapted from http://stackoverflow.com/questions/20595340/loading-a-tga-bmp-file-in-c-opengl
    void load(const char* FilePath)
    {
        std::fstream hFile;
        
        //search for file by going up file directory tree up to 5 times
        int attempts = 0;
        string nfilepath = FilePath;
        while (!hFile.is_open() && attempts < 5) {
            hFile.open(nfilepath.c_str(), std::ios::in | std::ios::binary);
            nfilepath = "../" + nfilepath;
            attempts += 1;
        }
        if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");
        
        hFile.seekg(0, std::ios::end);
        int Length = (int)hFile.tellg();
        hFile.seekg(0, std::ios::beg);
        std::vector<uint8_t> FileInfo(Length);
        hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);
        
        if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
        {
            hFile.close();
            throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
        }
        
        if (FileInfo[28] != 24 && FileInfo[28] != 32)
        {
            hFile.close();
            throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
        }
        
        BitsPerPixel = FileInfo[28];
        width = FileInfo[18] + (FileInfo[19] << 8);
        height = FileInfo[22] + (FileInfo[23] << 8);
        uint32_t pixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
        uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
        pixels.resize(size);
        
        hFile.seekg (pixelsOffset, std::ios::beg);
        hFile.read(reinterpret_cast<char*>(pixels.data()), size);
        hFile.close();
    }
    
};

///////////////////////////////////////////////////////////////////////////////
struct MyApp : public App
{
    Shader* shader;
    GLuint positionID, colorID, textureCoordinateID, normalID;
    GLuint mModelID, mProjectionID, mViewID;
    GLuint vao, vbo;
    Vertex mSphere[SPHERE_TOTAL_VERTICES];

    MyApp() { init(); }
    
    void init()
    {
        
        Bitmap img("/Users/josegallegos/Documents/workspace/OpenGL/OpenGL/resources/earth.bmp");
        
        int tw = img.width;
        int th = img.height;
        
        createSphere( SPHERE_RADIUS, SPHERE_SLICES, SPHERE_STACKS );
        /*-----------------------------------------------------------------------------
         *  CREATE THE SHADER
         *-----------------------------------------------------------------------------*/
        shader = new Shader( vert, frag );
        shader->bind();
        positionID = glGetAttribLocation( shader->id(), "position" );
        colorID = glGetAttribLocation( shader->id(), "color" );
        textureCoordinateID = glGetAttribLocation( shader->id(), "textureCoordinate" );
        normalID = glGetAttribLocation( shader->id(), "normal");
        
        // Get uniform locations
        mModelID = glGetUniformLocation( shader->id(), "model" );
        mViewID = glGetUniformLocation( shader->id(), "view" );
        mProjectionID = glGetUniformLocation( shader->id(), "projection" );
        
        /*-----------------------------------------------------------------------------
         *  CREATE THE VERTEX ARRAY OBJECT
         *-----------------------------------------------------------------------------*/
        glGenVertexArrays( 1, &vao );
        glBindVertexArray( vao );
        
        /*-----------------------------------------------------------------------------
         *  CREATE THE VERTEX BUFFER OBJECT
         *-----------------------------------------------------------------------------*/
        glGenBuffers(1, &vbo);
        glBindBuffer( GL_ARRAY_BUFFER, vbo);
        glBufferData( GL_ARRAY_BUFFER, SPHERE_TOTAL_VERTICES * sizeof(Vertex), mSphere, GL_STATIC_DRAW );
        
        /*-----------------------------------------------------------------------------
         *  ENABLE VERTEX ATTRIBUTES
         *-----------------------------------------------------------------------------*/
        glEnableVertexAttribArray(positionID);
        glEnableVertexAttribArray(colorID);
        glEnableVertexAttribArray(textureCoordinateID);
        glEnableVertexAttribArray(normalID);
        
        // Tell OpenGL how to handle the buffer of data that is already on the GPU
        glVertexAttribPointer( positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0 );
        glVertexAttribPointer( colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof( Vertex, color ) );
        glVertexAttribPointer( textureCoordinateID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof( Vertex, texture ) );
        glVertexAttribPointer( normalID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof( Vertex, normal ) );
        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        
        glBindVertexArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        
        /*-----------------------------------------------------------------------------
         *  Allocate Memory on the GPU
         *-----------------------------------------------------------------------------*/
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
        
        /*-----------------------------------------------------------------------------
         *  Load data onto GPU
         *-----------------------------------------------------------------------------*/
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tw, th, GL_BGR, GL_UNSIGNED_BYTE, img.pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    }
    //-----------------------------------------------------------------------------
    
    void createSphere(GLfloat fRadius, GLint iSlices, GLint iStacks)
    {
        GLfloat drho = (GLfloat)(PI) / (GLfloat) iStacks;
        GLfloat dtheta = 2.0f * (GLfloat)(PI) / (GLfloat) iSlices;
        GLfloat ds = 1.0f / (GLfloat) iSlices;
        GLfloat dt = 1.0f / (GLfloat) iStacks;
        GLfloat t = 0.0f;
        GLfloat s = 1.0f;
        GLint i, j;     // Looping variables
        size_t vertexCount = 0;
        for (i = 0; i < iStacks; i++)
        {
            GLfloat rho = (GLfloat)i * drho;
            GLfloat srho = (GLfloat)(sin(rho));
            GLfloat crho = (GLfloat)(cos(rho));
            GLfloat srhodrho = (GLfloat)(sin(rho + drho));
            GLfloat crhodrho = (GLfloat)(cos(rho + drho));
            s = 0.0f;
            for ( j = 0; j <= iSlices; j++)
            {
                GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;
                GLfloat stheta = (GLfloat)(-sin(theta));
                GLfloat ctheta = (GLfloat)(cos(theta));
                
                GLfloat x = stheta * srho;
                GLfloat y = ctheta * srho;
                GLfloat z = crho;
                
                mSphere[vertexCount].texture = glm::vec2( s, t );
                mSphere[vertexCount].normal = glm::vec3( x, y, z );
                mSphere[vertexCount++].position = glm::vec3(x * fRadius, y * fRadius, z * fRadius);
                
                x = stheta * srhodrho;
                y = ctheta * srhodrho;
                z = crhodrho;
                mSphere[vertexCount].texture = glm::vec2( s, t + dt );
                s -= ds;
                mSphere[vertexCount].normal = glm::vec3( x, y, z );
                mSphere[vertexCount++].position = glm::vec3(x * fRadius, y * fRadius, z * fRadius);
            }
            
            t += dt;
        }
    }
    
    //-----------------------------------------------------------------------------
    void onDraw()
    {
        static float time = 0.0;
        glEnable( GL_TEXTURE_2D );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glBindVertexArray( vao );
        
        //time += 0.05f;
        time += 0.005f;
        
        glm::mat4 view = glm::lookAt( glm::vec3( 0.0f, 0.0f, 2.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
        glUniformMatrix4fv( mViewID, 1, GL_FALSE, glm::value_ptr( view ) );
        
        glm::mat4 projection = glm::perspective( glm::pi<float>() * 20.0f, DISPLAY_RATIO, 0.1f, -10.0f );
        glUniformMatrix4fv( mProjectionID, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glm::mat4 translate = glm::translate( glm::mat4(), glm::vec3( 0.0f, 0.0f, 0.0f ) );
        glm::mat4 rotate = glm::rotate( glm::mat4(), glm::pi<float>() * 35, glm::vec3( 1.0f, .235f, 0.0f ) );
        glm::mat4 rotate2 = glm::rotate( glm::mat4(), 2 * glm::pi<float>() * time, glm::vec3( 0.0f, 0.0f, 1.0f ) );
        rotate *= rotate2;
        glm::mat4 scale = glm::scale( glm::mat4(), glm::vec3( 1.0f ) );
        glm::mat4 model = translate * rotate * scale;
        glUniformMatrix4fv( mModelID, 1, GL_FALSE, glm::value_ptr( model ) );
        
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, SPHERE_TOTAL_VERTICES );

        
        glBindVertexArray( 0 );
        glDisable( GL_TEXTURE_2D );
    }
    //-----------------------------------------------------------------------------
    void onKeyDown( int key, int action )
    {
        // do nothing
        if ( key == GLFW_KEY_1 )
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
        }
        else if( key == GLFW_KEY_2 )
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }
        else
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
    }
    //-----------------------------------------------------------------------------
};
///////////////////////////////////////////////////////////////////////////////
int main(int argc, const char ** argv)
{
    MyApp app;
    app.start();
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
