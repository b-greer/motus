#version 150

#define SPEED 3.0
#define OFFSET 40.0

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;
// this is the end of the default functionality

uniform float time;
uniform float offset;

// this is something we're creating for this shader
out Vertex {
  vec4 pos; 
  vec4 normal;
  vec4 color;
  vec2 texCoord;
} vertex;
 
out vec2 v_texcoord;
out vec3 v_viewSpaceNormal;
out vec3 v_viewSpacePosition;

// These are passed in from OF programmable renderer
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

void main() {

	mat4 normalMatrix =  transpose(inverse(modelViewMatrix));

    v_viewSpaceNormal = normalize((normalMatrix * normal).xyz);
	v_viewSpacePosition = (modelViewMatrix * position).xyz;
	v_texcoord = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;

	vertex.pos = position;
	vertex.normal = normal;
	vertex.color =  color;
	vertex.texCoord = texcoord;
   
   //gl_Position = modelViewProjectionMatrix * position;

  gl_Position = position;

  
}