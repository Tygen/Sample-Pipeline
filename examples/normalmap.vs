#version 330

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat3 uNormalMatrix;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBitangent;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fTangent;
out vec3 fBitangent;

void main()
{
    // Your code goes here.
    
    // OpenGL requires that we put something for gl_Position.
    // This is wrong but will at least compile
    //convert T B N to eyesapce
    //texture
    fTexCoord = vTexCoord;  //Pass the texture coordinate vTexCoord out unchanged.

    //fPos
    vec4 temp_Pos = vec4(vPos, 1.0);
    temp_Pos = uViewMatrix * temp_Pos;      //temp_Pos now is eye space
    fPos = vec3(temp_Pos[0], temp_Pos[1], temp_Pos[2]);

    //fTangent
    fTangent = vec3(uViewMatrix*vec4(vTangent,0));

    //fBitangent
    fBitangent = vec3(uViewMatrix*vec4(vBitangent,0));

    //fNormal
    fNormal = uNormalMatrix * vNormal;

    //gl_Position
    gl_Position = uProjectionMatrix * temp_Pos;

}
