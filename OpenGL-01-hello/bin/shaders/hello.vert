#version 130 // to support in/out instead of old-style varying

// input attributes of vertices
in vec3 position;
in vec3 normal;		// we are using this for vertex color

// the second output = input to fragment shader
out vec3 vertex_color;

// uniform variables
uniform float theta;

void main()
{
	// gl_Position: a built-in output variable that should be written in main()
	gl_Position = vec4( position, 1 );

	// rotation vector
	vec2 r = vec2(cos(theta),sin(theta));

	// rotate the vertices
	gl_Position.x = dot( position.xy, vec2(r.x,-r.y) );
	gl_Position.y = dot( position.xy, r.yx );	// swizzling for easy access

	// another output passed via varying variable
	vertex_color = normal;	// pass the color in norm to the vertex color output
}