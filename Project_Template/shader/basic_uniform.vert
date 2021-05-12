#version 460

const float NUM = 1.23456789;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;

layout (location = 0) in vec3 AnimVertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec4 AnimPosition;
out vec3 Normal;

out vec3 ShadowNormal;
out vec4 ShadowCoord;

// Frequency of animation, the higher the number the more the texture will move around the model
uniform float Freq = 2.5;
// Can be summed up as the speed of the texture moving around the object
uniform float AnimVelocity = 2.5;
// Increases the appearance of distortion
uniform float Amp = 0.6;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform mat4 ShadowMatrix;

uniform int Pass;

/*layout( xfb_buffer = 0, xfb_offset = 0)*/ out vec3 Position;
/*layout( xfb_buffer = 1, xfb_offset = 0)*/ out vec3 Velocity;
/*layout( xfb_buffer = 2, xfb_offset = 0)*/ out float Age;

// Is a left over from when this shader was different, this shader never asks for transparency 
out float Transp;
out vec2 TexCoord;

uniform float Time;
uniform float DeltaT;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform vec3 Emitter = vec3(0);
uniform mat3 EmitterBasis;
uniform float ParticleSize;

uniform mat4 MV;
uniform mat4 Proj;

uniform sampler1D RandomTex;

const vec3 offsets[] = vec3[](vec3(-0.5,-0.5,0), vec3(0.5,-0.5,0), vec3(0.5,0.5,0), vec3(-0.5,-0.5,0), vec3(0.5,0.5,0.),vec3(-0.5,0.5,0));

const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

vec3 randomInitialVelocity(){
float theta = mix(0.0, NUM / 8.0, texelFetch(RandomTex, 3 * gl_VertexID, 0).r);
float phi = mix(0.0, 2.0 * NUM, texelFetch(RandomTex, 3 * gl_VertexID + 1, 0).r);
float velocity = mix(1.25, 1.5, texelFetch(RandomTex, 3 * gl_VertexID + 2, 0).r);
vec3 v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
return normalize(EmitterBasis * v) * velocity;
}

void update(){
if(VertexAge < 0 || VertexAge > ParticleLifetime) {
Position = Emitter;
Velocity = randomInitialVelocity();
if(VertexAge < 0) Age = VertexAge + DeltaT;
else Age = (VertexAge - ParticleLifetime) + DeltaT;
}
else
{
// If it doesn’t equal the above equation than Position is made to equal VertexPosition + VertextVelocity * DeltaT, this is a safeguard as Position is registered later on in this shader
Position = VertexPosition + VertexVelocity * DeltaT;
Velocity = VertexVelocity + Accel * DeltaT;
Age = VertexAge + DeltaT;
}
}

void render() {
// Is a left over from when this shader was different, this shader never asks for transparency 
Transp = 0.0;
vec3 posCam = vec3(0.0);
if(VertexAge >= 0.0) {
posCam = (MV * vec4(VertexPosition,1)).xyz + offsets[gl_VertexID]* ParticleSize;
// Is a left over from when this shader was different, this shader never asks for transparency 
Transp = clamp(1.0 - VertexAge / ParticleLifetime, 0, 1);
}
//TexCoord = texCoords[gl_VertexID] is what leads to the surface animation, without this the program still loads but the texture coordinates are never updated
TexCoord = texCoords[gl_VertexID];
gl_Position = Proj * vec4(posCam,1);
}

void main(){

// Establishes pos as AnimVertexPosition
vec4 pos = vec4(AnimVertexPosition,1.0);
// Establishes u making is equal Freq * pos.x - AnimVelocity * Time
// u is the frequency * the x of pos - the speed of the moving texture * time
float u = Freq * pos.x - AnimVelocity * Time;
// to work out the y position amp and the sin of u is done as an equation 
pos.y = Amp * sin(u);
// n equals 0 initially
vec3 n = vec3(0.0);
// the x and y of n are a normalised version of the equation (vec2(cos(u),1.0))
n.xy = normalize(vec2(cos(u), 1.0));
AnimPosition = ModelViewMatrix * pos;
// Normal = NormalMartix times n
Normal = NormalMatrix * n;
// TexCoord = VertexTexCoord
TexCoord = VertexTexCoord;
gl_Position = MVP * pos;

Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
ShadowNormal = normalize(NormalMatrix * VertexNormal);
ShadowCoord = ShadowMatrix * vec4(VertexPosition,1.0);

if(Pass == 1)
update();
else
render();
}