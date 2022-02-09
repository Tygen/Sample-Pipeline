#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat3 uNormalMatrix;

uniform float uTime;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexCoord;

void main()
{
    // Your code goes here.
    
    // OpenGL requires that we put something for gl_Position.
    // This is wrong but will at least compile.
    //gl_Position = vec4( 1.0, 1.0, 1.0, 1.0 );

    //texture
    fTexCoord = vTexCoord + round(0.5 * sin(uTime));  //Pass the texture coordinate vTexCoord out unchanged.

    //fPos
    vec4 temp_Pos = vec4(vPos, 1.0);
    temp_Pos = uViewMatrix * temp_Pos + 0.5 * sin(uTime);      //temp_Pos now is eye space
    temp_Pos[1] = temp_Pos[1] + 2.5 * sin(uTime);
    fPos = vec3(temp_Pos[0], temp_Pos[1], temp_Pos[2]);

    //fNormal
    fNormal = uNormalMatrix * vNormal;

    //gl_Position
   gl_Position = uProjectionMatrix * temp_Pos;

}