#version 460
#define PI 3.14159265
in float Transp;
in vec2 TexCoord;

in vec3 Position;
in vec3 Normal;

//
in vec4 ShadowCoord;
//

////
uniform vec4 Color;
uniform sampler2D NoiseTex;
uniform vec4 SkyColor = vec4(0.3, 0.3, 0.9, 1.0);
uniform vec4 CloudColor = vec4(1.0, 1.0, 1.0, 1.0);


////

layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D Tex2;
uniform vec3 Ld;
uniform vec3 f;
out vec3 LightIntensity;

uniform struct LightInfo {
  vec4 LightPosition; 
  vec3 La;
  vec3 L; 
  //
  vec3 Intensity;
  //
 } Light;

 uniform struct MaterialInfo {
vec3 Ka;
 vec3 Kd; 
 vec3 Ks; 
 float Shininess; 
} Material;

//
uniform sampler2DShadow ShadowMap;
layout (location = 1) out vec4 NewFragColor;
//
layout (location = 0) out vec4 FragColor;
//phongModelDiffAndSpec might have to get rid of blinnPhong
vec3 phongModelDiffAndSpec()
{
vec3 n = Normal;
vec3 s = normalize(vec3(Light.LightPosition) - Position);
vec3 v = normalize(-Position.xyz);
vec3 r = reflect(-s,n);
float sDotN = max(dot(s,n),0.0);
vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
vec3 spec = vec3(0.0);
if (sDotN > 0.0)
spec = Light.Intensity * Material.Ks * pow(max(dot(r,v),0.0),Material.Shininess);
return diffuse + spec;
}
//phongModelDiffAndSpec might have to get rid of blinnPhong

//
subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine(RenderPassType)
void shadeWithShadow()
{
vec3 ambient = Light.Intensity * Material.Ka;
vec3 diffAndSpec = phongModelDiffAndSpec();

float shadow = 1.0;
if (ShadowCoord.z >= 0) {
shadow = textureProj(ShadowMap, ShadowCoord);
}
NewFragColor = vec4(diffAndSpec * shadow + ambient, 1.0);

NewFragColor = pow(NewFragColor, vec4(1.0 / 2.2));
}

subroutine (RenderPassType)
void recordDepth()
{
}

//

//blinnPhong Model
//vec3 blinnPhong(vec3 position, vec3 n ) {
//vec4 texColor1 = texture(Tex1, TexCoord);
//vec4 texColor2 = texture(Tex2, TexCoord);
//vec3 col = mix(texColor1.rgb, texColor2.rgb, texColor2.a);
//vec3 ambient = col * Light.La;
//vec3 s = Light.LightPosition.xyz - col;
//float sDotN = (dot(s,n));
//vec3 diffuse = col * sDotN;
//vec3 spec = vec3(1.0);
//if( sDotN > 0.0 )
//{
//vec3 v = normalize(-position.xyz);
//vec3 h = normalize( v + s );
//spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
//}
//return ambient + Light.L * (diffuse + spec);
//}
//blinnPhong Model

uniform sampler2D ParticleTex;

void main()
{

//For Texture
FragColor = texture(ParticleTex, TexCoord);
//Remove if final texture isn't transparent to some extent
//FragColor.a *= Transp;
//Remove if final texture isn't transparent to some extent
//For Texture

//For Noise
//vec4 noise = texture(NoiseTex, TexCoord);
//float t = (cos(noise.a * PI) + 1.0) / 2.0;
//vec4 color = mix(SkyColor, CloudColor, t);
//FragColor = vec4(color.rgb, 1.0);
//For Noise

//
RenderPass();
//
}
