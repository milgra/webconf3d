

    #ifndef opengl_h
    #define opengl_h


	#include <stdio.h>
	#include <stdlib.h>
    #ifdef IOS
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
    #elif defined OSX
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    #elif defined ANDROID
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
	#elif defined ASMJS
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
    #endif

    void    ogl_errors( const char* place );

	GLuint ogl_shader_create_from_file( const char* vertex_source , const char* fragment_source , const char** uniform_structure , const char** attribute_structure , GLint* uniform_locations );

	GLuint ogl_shader_create( const char* vertex_source , const char* fragment_source , const char** uniform_structure , const char** attribute_structure , GLint* uniform_locations );

	void    ogl_shader_delete( GLuint program );

	void    ogl_vertexbuffer( GLuint* buffername );

	void    ogl_texture_create_linear( GLuint* texturename , GLsizei width , GLsizei height );

	void    ogl_texture_create_nearest( GLuint* texturename , GLsizei width , GLsizei height );

	void    ogl_framebuffer_with_texture( GLuint* buffername , GLuint* texturename , GLsizei width , GLsizei height );

	void    ogl_test(void);


	void    ogl_enableva_f2u1(void);

	void    ogl_enableva_f2f2(void);

	void    ogl_enableva_f3f2(void);

	void    ogl_enableva_f3f3(void);

	void    ogl_enableva_f3u4(void);


	float   ogl_color_float_from_argbuint32( uint32_t color );
    
	float   ogl_color_float_from_rgbauint32( uint32_t color );


//	uint32_t ogl_color_from_floats_reversed( float r , float g , float b , float a );
//
//	uint32_t ogl_color_from_floats( float r , float g , float b , float a );
//
//	float   ogl_color_float_from_floats_reversed( float r , float g , float b , float a );
//
//	float   ogl_color_float_from_floats( float r , float g , float b , float a );
//
//	void    ogl_color_float_from_uint( float* r , float* g , float* b , float* a , uint32_t color );
//
//
	// void    ogl_draw_unit_quad( );
    //
    void ogl_texture_update_rgba( GLint texture , GLint width , GLint height , uint8_t* data );
    //
    //	GLuint ogl_shader_create_blend( GLint* uniforms );
    //
    //	GLuint ogl_shader_create_blur( GLint* uniforms );


    #endif
