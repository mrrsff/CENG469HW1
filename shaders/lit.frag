#version 330 core

in vec4 color;
out vec4 fragColor;

void main(void)
{
	// Debug z-buffer
	// fragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
	fragColor = color;
}
