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

//*******************************************************************
// global constants
static const char*	window_name = "cgbase - circle";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";
static const uint	MIN_TESS = 3;		// minimum tessellation factor (down to a triangle)
static const uint	MAX_TESS = 256;		// maximum tessellation factor (up to 256 triangles)
uint				NUM_TESS = 7;		// initial tessellation factor of the circle as a polygon

//*******************************************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 1280, 720 );	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer	= 0;	// ID holder for vertex buffer
GLuint	index_buffer	= 0;	// ID holder for index buffer

//*******************************************************************
// global variables
int		frame = 0;				// index of rendering frames
vec4    solid_color = vec4( 1.0f, 0.5f, 0.5f, 1.0f );
float	radius = 1.0f;
bool    bUseSolidColor = false;
bool	bUseIndexBuffer = true;
bool	bWireframe = false;			// this is the default

//*******************************************************************
// holder of vertices and indices
std::vector<vertex>	vertex_list;	// host-side vertices
std::vector<uint>	index_list;		// host-side indices

//*******************************************************************
void update()
{
	// update simulation
	float t = float(glfwGetTime())*0.5f;

	radius = 0.7f+(cos(t)+1.0f)*0.15f;	// simple animation
	mat4 rotation_matrix =				// explained later (in the transformation lecture)
	{
		cos(t), -sin(t), 0, 0,
		sin(t), cos(t), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "bUseSolidColor" );	if(uloc>-1) glUniform1i( uloc, bUseSolidColor );
	uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, solid_color );	// pointer version
	uloc = glGetUniformLocation( program, "aspect_ratio" );		if(uloc>-1) glUniform1f( uloc, window_size.x/float(window_size.y) );
	uloc = glGetUniformLocation( program, "radius" );			if(uloc>-1) glUniform1f( uloc, radius );
	uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, rotation_matrix );
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
	if(bUseIndexBuffer)
	{
		if(index_buffer) glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );		
		glDrawElements( GL_TRIANGLES, index_list.size(), GL_UNSIGNED_INT, nullptr );
	}
	else
	{
		glDrawArrays( GL_TRIANGLES, 0, NUM_TESS*3 ); // NUM_TESS = N
	}

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
	printf( "- press '+/-' to increase/decrease tessellation factor (min=%d, max=%d)\n", MIN_TESS, MAX_TESS );
	printf( "- press 'i' to toggle between index buffering and simple vertex buffering\n" );
	printf( "- press 'w' to toggle wireframe\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	void update_vertex_buffer( uint N );	// forward declaration
	void update_circle_vertices( uint N );	// forward declaration
	
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_KP_ADD||(key==GLFW_KEY_EQUAL&&(mods&GLFW_MOD_SHIFT)))
		{
			if(NUM_TESS>=MAX_TESS) return;
			update_circle_vertices(++NUM_TESS);
			update_vertex_buffer(NUM_TESS);
			printf( "> NUM_TESS = % -4d\r", NUM_TESS );
		}
		else if(key==GLFW_KEY_KP_SUBTRACT||key==GLFW_KEY_MINUS)
		{
			if(NUM_TESS<=MIN_TESS) return;
			update_circle_vertices(--NUM_TESS);
			update_vertex_buffer(NUM_TESS);
			printf( "> NUM_TESS = % -4d\r", NUM_TESS );
		}
		else if(key==GLFW_KEY_W)
		{
			bWireframe = !bWireframe;
			glPolygonMode( GL_FRONT_AND_BACK, bWireframe ? GL_LINE:GL_FILL );
			printf( "> using %s mode\n", bWireframe ? "wireframe" : "solid" );
		}
		else if(key==GLFW_KEY_I)
		{
			bUseIndexBuffer = !bUseIndexBuffer;
			update_vertex_buffer(NUM_TESS);
			printf( "> using %s buffering\n", bUseIndexBuffer?"index":"vertex" );
		}
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

void update_vertex_buffer( uint N )
{
	// clear and create new buffers
	if(vertex_buffer)	glDeleteBuffers( 1, &vertex_buffer );	vertex_buffer = 0;
	if(index_buffer)	glDeleteBuffers( 1, &index_buffer );	index_buffer = 0;

	// check exceptions
	if(vertex_list.empty()){ printf("[error] vertex_list is empty.\n"); return; }

	// create buffers
	if(bUseIndexBuffer)
	{
		index_list.clear();
		for( uint k=0; k < N; k++ )
		{
			index_list.push_back(0);	// the origin
			index_list.push_back(k+1);
			index_list.push_back(k+2);
		}

		// generation of vertex buffer: use vertex_list as it is
		glGenBuffers( 1, &vertex_buffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*vertex_list.size(), &vertex_list[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*index_list.size(), &index_list[0], GL_STATIC_DRAW );
	}
	else
	{
		std::vector<vertex> triangle_vertices;
		for( uint k=0; k < N; k++ )
		{
			triangle_vertices.push_back(vertex_list.front());	// the origin
			triangle_vertices.push_back(vertex_list[k+1]);
			triangle_vertices.push_back(vertex_list[k+2]);
		}

		// generation of vertex buffer: use triangle_vertices instead of vertex_list
		glGenBuffers( 1, &vertex_buffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*triangle_vertices.size(), &triangle_vertices[0], GL_STATIC_DRAW);
	}
}

void update_circle_vertices( uint N )
{
	vertex_list.clear();

	// define the position of four corner vertices
	vertex_list.push_back( { vec3(0,0,0), vec3(0.0f,0.0f,-1.0f), vec2(0.5f)} );	// the origin
	for( uint k=0; k <= N; k++ )
	{
		float t = PI*2.0f/float(N)*float(k);
		float c=cos(t), s=sin(t);
		vertex_list.push_back( { vec3(c,s,0.0f), vec3(0.0f,0.0f,-1.0f), vec2(c*0.5f+0.5f,s*0.5f+0.5f)} );
	}
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth( 1.0f );
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	
	// define the position of four corner vertices
	update_circle_vertices( NUM_TESS );

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer( NUM_TESS );

	return true;
}

void user_finalize()
{
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
