#version 330

// Eye-space.
in vec3 fPos;
in vec3 fNormal;
in vec2 fTexCoord;

uniform mat3 uNormalMatrix;

uniform samplerCube uEnvironmentTex;

uniform mat4 uViewMatrix;

uniform struct Material {
    vec3 color_ambient;     //Ka
    vec3 color_diffuse;     //
    vec3 color_specular;    //Ks
    float shininess;        //n
    
    bool reflective;
    vec3 color_reflect;
    
    bool refractive;
    vec3 color_refract;
    float index_of_refraction;
    
    bool use_diffuse_texture;       //
    sampler2D diffuse_texture;
} material;

struct Light {
    // Eye-space.
    vec3 position;
    vec3 color;
    vec3 color_ambient;
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
    //the overall goal here is to output an color

    vec3 my_color = vec3(0.0, 0.0, 0.0);		//initialized, use to return
	vec3 ambient_light = vec3(0.0, 0.0, 0.0);	//Ambient lighting
	vec3 diffuse_light = vec3(0.0, 0.0, 0.0);	//Diffuse lighting
	vec3 specular_lighting = vec3(0.0, 0.0, 0.0);	//Specular lighting
	vec3 reflection_color = vec3(0.0, 0.0, 0.0);
    vec3 summary_color = vec3(0.0, 0.0, 0.0);

    vec3 normalized_normal = vec3(0.0, 0.0, 0.0);
	vec3 normalized_L = vec3(0.0, 0.0, 0.0);
	float dot_NL; 

	vec3 reflected = vec3(0.0, 0.0, 0.0);
    vec3 eye_position = vec3(0.0, 0.0, 0.0);
	float dot_VR;
    
    for (int i = 0; i < num_lights; i++) {
        ///direct illumination
        //Ambient lighting
        ambient_light = material.color_ambient * lights[i].color_ambient;	

        //diffuse lighting process
        normalized_normal = normalize(fNormal);
        normalized_L = normalize( lights[i].position - fPos);		//L is the (normalized) vector from the surface position to the light's position.
        dot_NL = dot(normalized_normal, normalized_L);
        if(material.use_diffuse_texture == false){
            diffuse_light = material.color_diffuse * lights[i].color * dot_NL;
            //diffuse_light = material.color_diffuse * texture( material.diffuse_texture, fTexCoord ) * lights[i].color * dot_NL;
        }else{
            diffuse_light = material.color_diffuse * texture( material.diffuse_texture, fTexCoord ) * lights[i].color * dot_NL;
        }

        //specular lighting
        reflected = -normalized_L + 2 * dot(normalized_L, normalized_normal) * normalized_normal;
        dot_VR = dot(normalize(eye_position - fPos), normalize(reflected));
        dot_VR = max(0.0, dot_VR);
        specular_lighting = material.color_specular * lights[i].color * pow(dot_VR, material.shininess); 

        ///indirect illumination
        
        if (dot_NL > 0){
            summary_color += ambient_light + (diffuse_light + specular_lighting);
        } else if(dot_NL == 0.0){
            summary_color += ambient_light + (diffuse_light);
        }else {
            summary_color += ambient_light;
        }

    }
    
    //reflection
    if (material.reflective)
    {
        vec3 eye_to_surface = fPos - eye_position;		//original "vector"
                                                            ///reflect
        normalized_normal = normalize(fNormal);

        eye_to_surface = - reflect(-eye_to_surface, normalized_normal);

        vec4 temp_ray = vec4(eye_to_surface, 0.0);
        temp_ray = inverse(uViewMatrix) * temp_ray;
        
        eye_to_surface =  vec3(temp_ray[0], temp_ray[1], temp_ray[2]);

        reflection_color = texture( uEnvironmentTex, eye_to_surface ).rgb;
    }

    my_color =  material.color_reflect * reflection_color + summary_color;
    
    FragColor = vec4( my_color, 1.0 );
}
