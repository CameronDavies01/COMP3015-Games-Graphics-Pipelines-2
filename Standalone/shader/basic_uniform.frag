#version 460
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D Tex2;
//declare your uniform variables here

layout( location = 0 ) out vec4 FragColor;

uniform vec3 Ld;
uniform vec3 f;
out vec3 LightIntensity;

uniform struct LightInfo {
  // Light position in the eye coordinates
  vec4 LightPosition; 
  // Ambient light intensity
  vec3 La;
  // Diffuse and specular light intensity
  vec3 L; 
 } lights[3];

uniform struct MaterialInfo {
 // Ambient reflectivity
vec3 Ka;
 // Diffuse reflectivity
 vec3 Kd; 
 // Specular reflectivity
 vec3 Ks; 
 // Specular shininess
 float Shininess; 
} Material;

vec3 blinnPhong(vec3 position, vec3 n ) {
vec4 texColor1 = texture(Tex1, TexCoord);
vec4 texColor2 = texture(Tex2, TexCoord);
vec3 col = mix(texColor1.rgb, texColor2.rgb, texColor2.a);
vec3 ambient = col * lights[1].La;
vec3 s = lights[0].LightPosition.xyz - col;
float sDotN = (dot(s,n));
vec3 diffuse = col * sDotN;
vec3 spec = vec3(1.0);
if( sDotN > 0.0 )
{
vec3 v = normalize(-position.xyz);
vec3 h = normalize( v + s );
spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
}
return ambient + lights[2].L * (diffuse + spec);
}

void main()
{
FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1);
}