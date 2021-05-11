in vec4 Color;
//in vec3 Color
in vec4 FragColor;

void main()
{
   //FragColor = vec4(Color,1.0);
   FragColor = Color;
}

