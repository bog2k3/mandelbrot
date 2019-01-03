#version 330 core
 
in vec3 pos;
in vec2 uv;

out vec2 fragUV;
 
void main() {
	gl_Position = pos; 
	fragUV = uv;
}
