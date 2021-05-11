in vec3 VertexPosition;
in mat4 Proj;
in mat4 MV;

void main()
{
gl_Position = Proj * MV * vec4(VertexPosition, 1.0);
}
