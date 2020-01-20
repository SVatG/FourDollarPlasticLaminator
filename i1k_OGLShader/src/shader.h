//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//
/*
const static char *vertexShader=\
    "varying vec4 v;"
    "void main()"
    "{"
        "v=vec4(gl_Vertex.xy,64.0*gl_Color.xy);"
        "gl_Position=gl_Vertex;"
    "}";
*/
const static char *fragmentShader=\
    //"varying vec4 v;"
 "void main(){"
 "float f=dot(gl_Color.xyz,vec3(1,256,256*256))*0.3-4.0;"
 "f=mod(f,105.0);"
 "vec3 r=normalize(vec3(-7.0+0.01*gl_FragCoord.x,-5.0+0.01*gl_FragCoord.y,10.0));"
 "vec3 g=vec3(0,0,-10.0);"
 "float u=200;"
 "int c=0;"
 "while(u>0.1&&c<200) {"
 "vec3 p=g;"
 "float e=3.0+sin(p.x+1.5)+p.y+cos(f)*2.5+sin(p.z+f*10.0);"
 "float h=cosh(p.x)+sin(f)*sin(sin(p.y))+sin(p.z+f*15.0)+2.5+p.y;"
 "float j=3.0+cos(p.x+1.5)+sinh(-p.x+1.5)+p.y+cos(f)+sin(p.z+f*15.0);"
 "float k=cosh(p.x)-cosh(-p.x)+cosh(sin(p.y+sin(p.y)))+sin(p.z+f*30.0)+2.5+p.y;"
 "p.z+=f*3.0;"
 "p.x+=sin(f)*3.0+sin(p.z)*smoothstep(20.0,0.0,f);"
 "p-=floor(p);"
 "p-=0.5;"
 "float i=max(max(abs(p.x),abs(p.y)),abs(p.z))-(sin(f/3.0+1.55)+1.0)/5.0*smoothstep(20.0,15.0,f);"
 "float x=smoothstep(40.0,30.0,f);"
 "float y=smoothstep(55.0,45.0,f);"
 "float z=smoothstep(70.0,60.0,f);"
 "float w=smoothstep(75.0,70.0,f);"
 "float n=smoothstep(90.0,80.0,f);"
 "float t=smoothstep(100.0,90.0,f);"
 "u=i*x+e*(1.0-x)*y+h*(1.0-y)*z+j*(1.0-z)*n+i*(1.0-w)*t+(1.0-n)*k*t;"
 "g=g+r*u;"
 "c++;"
 "}"
 "gl_FragColor.rgb=vec3(0.4+smoothstep(70.0,95.0,f),smoothstep(30.0,40.0,f)*smoothstep(90.0,75.0,f),0.8*smoothstep(70.0,50.0,f))*c/75.0;"
 "}";
