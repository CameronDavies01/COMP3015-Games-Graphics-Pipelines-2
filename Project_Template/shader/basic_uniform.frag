#version 460

in vec2 TexCoord;

in vec3 Position;
in vec3 Normal;

uniform struct LightInfo {
  vec4 LightPosition; 
  vec3 La;
  vec3 L; 
  vec3 Intensity;
 } Light;

 uniform struct MaterialInfo {
vec3 Ka;
 vec3 Kd; 
 vec3 Ks; 
 float Shininess; 
} Material;

//uniform sampler2DShadow ShadowMap;

layout (location = 1) out vec4 NewFragColor;
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

//phongModelDiffAndSpec
subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;
subroutine(RenderPassType)
void shadeWithShadow()
{
vec3 ambient = Light.Intensity * Material.Ka;
vec3 diffAndSpec = phongModelDiffAndSpec();
float shadow = 1.0;

NewFragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
NewFragColor = pow(NewFragColor, vec4(1.0 / 2.2));
}
subroutine (RenderPassType)
void recordDepth()
{
}

uniform sampler2D ParticleTex;

void main()
{
FragColor = texture(ParticleTex, TexCoord);
RenderPass();
}
