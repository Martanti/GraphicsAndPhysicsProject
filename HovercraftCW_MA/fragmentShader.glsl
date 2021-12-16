#version 430 core

#define NO_DATA 0
#define TEXTURED 1
#define MATERIALS 2
#define COMBINED 3
#define SKYBOX 4

#define DIRECTIONAL 0
#define POINT_OF_LIGHT 1

//structures
struct SDirectionalLight
{
   vec3 m_vec3Ambient;
   vec3 m_vec3Diffuse;
   vec3 m_vec3Specular;
   vec3 m_vec3Direct;
};

struct  SMaterial
{
	vec3 m_vec3Ambient;
	vec3 m_vec3Diffuse;
	vec3 m_vec3Specular;
	vec3 m_vec3Emission;
	float m_fGloss;
};

//variables coming from vertex shader
in vec2 vec2TextureCoordinateExport;

in vec3 vec3NormalExport;

in vec3 vec3TextureCoordinateExport;

in vec3 vec3FragmentPosition;

//variables from backend
uniform SDirectionalLight directionalLight;
uniform SMaterial materialObject;

uniform uint uiVisualizationType;

uniform vec3 vec3CameraPosition;

uniform sampler2D Texture2D;

uniform samplerCube skybox;

uniform uint uiLighType;

uniform mat4 mat4View;
uniform mat4 mat4Model;

//output
out vec4 colorsOut;

void main(void)
{
    //to save some time and skip all the calculations
    switch(uiVisualizationType){

        case TEXTURED:
            colorsOut = texture( Texture2D , vec2TextureCoordinateExport);
            return;
            break;

        case SKYBOX:
            colorsOut = texture( skybox , vec3TextureCoordinateExport);
            return;
            break;

        case NO_DATA:
            colorsOut = vec4(1,0,1,1);
            return;
            break;
    }

    //general

    mat4 mat4NormalMatrix = transpose(inverse( mat4Model));
    vec3 vec3Normal = normalize( mat3(mat4NormalMatrix) * vec3NormalExport);
    
	vec3 vec3LightDirection;

    //specular (declaring early so it can be set in the switch statement and there wouldn't be a need to have two same  switch statemts)
    float fSpecularStrength = 0.4f;
    vec3 vec3ViewingDirection = normalize(vec3CameraPosition  - vec3FragmentPosition);
    vec3 vec3HalfwayDirecetion;

    switch(uiLighType){

        case POINT_OF_LIGHT:
            vec3LightDirection = normalize(vec3(3,5,0) - vec3FragmentPosition);
            vec3HalfwayDirecetion = normalize(vec3LightDirection + vec3ViewingDirection);
            break;

        case DIRECTIONAL:
        default:
            vec3LightDirection = normalize(directionalLight.m_vec3Direct);
            vec3HalfwayDirecetion = normalize(vec3LightDirection );
            break;
    }

    //ambient
	vec3 vec3AmbientColour = directionalLight.m_vec3Ambient * materialObject.m_vec3Ambient*2; //*2 to solve the dark half problem
    
    //diffuse
    float fDiffuseImpact = max( dot(vec3Normal, vec3LightDirection) , 0.f);
    vec3 vec3DiffuseColour = fDiffuseImpact* directionalLight.m_vec3Diffuse;

    //Blinn-Phong - continuation of specular
    float fSpecularImpact = pow( max(dot(vec3Normal, vec3HalfwayDirecetion) ,0.f) , 64);

    vec3 vec3SpecularLight = fSpecularStrength * fSpecularImpact *  directionalLight.m_vec3Specular;

    //final
    vec3 vec3ResultColour = ( vec3AmbientColour + vec3DiffuseColour + vec3SpecularLight ) *  materialObject.m_vec3Diffuse;
	
	switch(uiVisualizationType)
    {
        case MATERIALS:
            colorsOut = vec4(vec3ResultColour, 1);
            break;

        case COMBINED:
            colorsOut = vec4(vec3ResultColour * vec3( texture(Texture2D , vec2TextureCoordinateExport)), 1);
            break;

        default:
            colorsOut = vec4(1,0,1,1);
            break;
    }
}