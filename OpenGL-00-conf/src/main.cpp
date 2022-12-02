https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
https://powcoder.com
代写代考加微信 powcoder
Assignment Project Exam Help
Add WeChat powcoder
#include "GL/glad/glad.h"	// https://github.com/Dav1dde/glad
#include "GL/glfw/glfw3.h"	// http://www.glfw.org
#include <stdio.h>

#pragma comment( lib, "OpenGL32.lib" )		// link OpenGL32 library
#pragma comment( lib, "glfw3.lib" )			// static version
//#pragma comment( lib, "glfw3dll.lib" )	// dynamic version for other VC version

void main()
{
	printf( "Hello OpenGL!\n\n" );

	// initialization
	if(!glfwInit()){ printf( "[error] failed in glfwInit()\n" ); return; }
	
	// create invisible window for OpenGL context
	glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
	GLFWwindow* window = glfwCreateWindow( 100, 100, "cgconf - Hello OpenGL", nullptr, nullptr );
	if(!window){ printf( "Failed to create GLFW window.\n" ); glfwTerminate(); return; }

	// make the current context and load GL extensions
	glfwMakeContextCurrent(window);
	if(!gladLoadGL()){ printf( "init_extensions(): Failed in gladLoadGLLoader()\n" ); glfwTerminate(); return; }

	// check renderer and vendor
	printf( "You are using\n" );
	printf( " - OpenGL %s\n", glGetString(GL_VERSION) );
	printf( " - OpenGL Shading Language %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );
	printf( " - GPU: %s\n", glGetString(GL_RENDERER) );
	printf( " - Vendor: %s\n", glGetString(GL_VENDOR) );
	printf( "\n" );

	glfwDestroyWindow(window);
	glfwTerminate();
}

