#version 330

// Eye-space.
in vec3 fPos;
in vec3 fNormal;
in vec2 fTexCoord;

uniform mat3 uNormalMatrix;

uniform struct Material {
    vec3 color;
    float shininess;
    int bands;
    
    bool use_diffuse_texture;
    sampler2D diffuse_texture;
} material;

struct Light {
    // Eye-space.
    vec3 position;
    float color;
    float color_ambient;
};
const int MAX_LIGHTS = 5;
uniform Light lights[ MAX_LIGHTS ];

uniform int num_lights;

// gl_FragColor is old-fashioned, but it's what WebGL 1 uses.
// From: https://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel
layout(location = 0) out vec4 FragColor;

void main()
{
    // Your code goes here.

    vec3 brightness_F = vec3(0.0, 0.0, 0.0);        //a summary value
    vec3 eye_position = vec3(0.0, 0.0, 0.0);
    vec3 normalized_normal = vec3(0.0, 0.0, 0.0);
	vec3 normalized_L = vec3(0.0, 0.0, 0.0);
    vec3 reflected = vec3(0.0, 0.0, 0.0);
    vec3 my_color = vec3(0.0, 0.0, 0.0);

    float dot_NL; 
    float dot_VR;
    

    for (int i = 0; i < num_lights; i++) {
        normalized_normal = normalize(fNormal);
        normalized_L = normalize( lights[i].position - fPos);
        dot_NL = dot(normalized_normal, normalized_L);

        reflected = -normalized_L + 2 * dot(normalized_L, normalized_normal) 
            * normalized_normal;
        dot_VR = dot(normalize(eye_position - fPos), normalize(reflected));
        dot_VR = max(0.0, dot_VR);      //? necessary? 

        brightness_F += lights[i].color_ambient + (lights[i].color * dot_NL 
            + lights[i].color * pow(dot_VR, material.shininess));
    }

    brightness_F = min( floor( brightness_F * brightness_F * material.bands), material.bands - 1 )/( material.bands - 1);

    if(material.use_diffuse_texture == false){
        my_color = material.color * brightness_F;
    }else{
        my_color = material.color * texture( material.diffuse_texture, fTexCoord ).rgb
            * brightness_F;
    }
    
    FragColor = vec4( my_color, 1.0 );
}
