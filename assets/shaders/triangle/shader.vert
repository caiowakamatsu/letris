#version 460

layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 col;
//layout(location = 2) in vec3 normal;

out vec3 c_col;
out vec3 c_normal;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);
    c_col = vec3(0.2, 0.2, 0.8);
    c_normal = vec3(0, 1, 0);
}