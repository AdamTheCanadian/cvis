#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec4 vertexColor;
void main()
{
   vertexColor = color;
   gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}