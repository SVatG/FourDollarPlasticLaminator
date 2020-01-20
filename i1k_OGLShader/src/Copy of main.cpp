//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include "glext.h"

//--------------------------------------------------------------------------//

const static char *vertexShader=\
    "varying vec4 v;"
    "void main()"
    "{"
        "v=vec4(gl_Vertex.xy,gl_Color.x,.5+.5*gl_Color.y);"
        "gl_Position=gl_Vertex;"
    "}";

const static char *fragmentShader=\
    "varying vec4 v;"
    "void main()"
    "{"
        #if 1
        // tunnel
        "float r=pow(pow(abs(v.x),18.0)+pow(abs(v.y),10.0),1.8/28.0);"
        "vec2 t=vec2(v.z+1./r,atan(v.x,v.y));"
        // texture
        "t=fract(2*t)-.5;"
        "vec4 col=(1-pow(dot(t.xy,t.xy),.3))*vec4(2,1.8,1.6,0)+vec4(.3,.2,.1,0)+.12*vec4(v.xy,0,0);"
        // final color
        "gl_FragColor=col/(2.0*r);"
        #else
        "gl_FragColor=vec4(v.xy,0.4,1.0);"
        #endif
    "}";

const static PIXELFORMATDESCRIPTOR pfd = {0,0,PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//--------------------------------------------------------------------------//

#ifdef DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
extern "C" int _fltused = 0;
void entrypoint( void )
#endif
{ 
    // create windows
    HDC hDC = GetDC( CreateWindow("edit", 0, WS_POPUP|WS_VISIBLE|WS_MAXIMIZE, 0,0,0,0,0,0,0,0) );

    // init opengl
    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
    wglMakeCurrent(hDC, wglCreateContext(hDC));

    // create shader
    int s, p;
    p = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();
    s = ((PFNGLCREATESHADERPROC)(wglGetProcAddress("glCreateShader")))(GL_VERTEX_SHADER);
    ((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(s, 1, &vertexShader, 0);
    ((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(s);
    ((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(p,s);
    s = ((PFNGLCREATESHADERPROC)(wglGetProcAddress("glCreateShader")))(GL_FRAGMENT_SHADER);	
    ((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(s, 1, &fragmentShader, 0);
    ((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(s);
    ((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(p,s);
    ((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(p);
    ((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(p);
    ShowCursor(0); 

    // run
    do
    { 
        const float t = .0025f*(float)(GetTickCount()&65535);
        glColor3f(t,sinf(.25f*t),0.0f);
        glRects(-1,-1,1,1);
        SwapBuffers(hDC); //wglSwapLayerBuffers( hDC, WGL_SWAP_MAIN_PLANE );
    }while ( !GetAsyncKeyState(VK_ESCAPE) );

    ExitProcess( 0 );

    #ifdef DEBUG
    return 0;
    #endif
}
