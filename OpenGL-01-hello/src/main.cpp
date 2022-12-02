https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

//*************************************
// global constants
static const char*	window_name	= "cgbase - Hello triangle!";
static const char*	vert_shader_path = "../bin/shaders/hello.vert";
static const char*	frag_shader_path = "../bin/shaders/hello.frag";

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 1024, 1024 );	// initial window size

//*************************************
// OpenGL objects
GLuint		program			= 0;	// ID holder for GPU program
GLuint		vertex_buffer	= 0;	// ID holder for vertex buffer

//*************************************
// global variables
int		frame = 0;		// index of rendering frames
vec4    solid_color = vec4( 1.0f, 0.5f, 0.5f, 1.0f );
bool    bUseSolidColor = false;

//*************************************
void update()
{
	// update simulation factor by time
	float theta = float(glfwGetTime())*0.5f;

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "theta" );			if(uloc>-1) glUniform1f( uloc, theta );
	uloc = glGetUniformLocation( program, "bUseSolidColor" );	if(uloc>-1) glUniform1i( uloc, bUseSolidColor );
	uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, solid_color );	// pointer version
}	

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex attributes to your shader program
	const char*	vertex_attrib[]	= { "position", "normal", "texcoord" };
	size_t		attrib_size[]	= { sizeof(vertex().pos), sizeof(vertex().norm), sizeof(vertex().tex) };
	for( size_t k=0, kn=std::extent<decltype(vertex_attrib)>::value, byte_offset=0; k<kn; k++, byte_offset+=attrib_size[k-1] )
	{
		GLuint loc = glGetAttribLocation( program, vertex_attrib[k] ); if(loc>=kn) continue;
		glEnableVertexAttribArray( loc );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glVertexAttribPointer( loc, attrib_size[k]/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) byte_offset );
	}

	// render vertices: trigger shader programs to process vertex data
	glDrawArrays( GL_TRIANGLES, 0, 3 ); // (topology, start offset, no. vertices)

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press 'd' to toggle between solid color and texture coordinates\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_D)
		{
			bUseSolidColor = !bUseSolidColor;
			printf( "> using %s\n", bUseSolidColor ? "solid color" : "texture coordinates as color" );
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS )
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		printf( "> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y) );
	}
}

void motion( GLFWwindow* window, double x, double y )
{
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth test

	// create a vertex array for triangles in default view volume: [-1~1, -1~1, -1~1]
	vertex vertices[] =
	{
		{ vec3(0.433f,-0.25f,0), vec3(1,0,0), vec2(0.0f) },  // {position, red color, ...}
		{ vec3(0.0f,0.5f,0), vec3(0,1,0), vec2(0.0f) },      // {position, green color, ...}
		{ vec3(-0.433f,-0.25f,0), vec3(0,0,1), vec2(0.0f) }  // {position, blue color, ...}
	};
	
	// create and update vertex buffer
	glGenBuffers( 1, &vertex_buffer ); // generate a buffer object
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer ); // notify GL to use the buffer
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW ); // copy data to GPU

	return true;
}

void user_finalize()
{
	// some clean-up code here
}

void main( int argc, char* argv[] )
{
	// initialization
	if(!glfwInit()){ printf( "[error] failed in glfwInit()\n" ); return; }

	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);
}
