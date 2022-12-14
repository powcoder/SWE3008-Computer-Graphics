#version 130

// inputs from vertex shader
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform bool bUseSolidColor;
uniform vec4 solid_color;

void main()
{
	fragColor = bUseSolidColor ? solid_color : vec4(tc.xy,0,1);
}