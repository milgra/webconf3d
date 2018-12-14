

    #ifndef floatbuffer_h
    #define floatbuffer_h

    #ifdef IOS
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
    #elif defined OSX
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    #elif defined ANDROID
    #include <EGL/egl.h>
    #include <GLES/gl.h>
	#elif defined ASMJS
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
    #endif
    #include "../core/mtmem.h"
    #include "../tools/math2.h"
    #include "../tools/math3.h"
    #include "../tools/math4.h"


    typedef struct floatbuffer_t floatbuffer_t;
    struct floatbuffer_t
    {
        GLfloat* data;
        uint32_t length;
        uint32_t length_real;

		uint8_t changed;
		matrix4_t projection;
    };


    floatbuffer_t*  floatbuffer_alloc( void );

	void            floatbuffer_dealloc( void* floatbuffer );

	void            floatbuffer_reset( floatbuffer_t* floatbuffer );

	void            floatbuffer_add( floatbuffer_t* floatbuffer , GLfloat data );

	void            floatbuffer_add4( floatbuffer_t* floatbuffer , GLfloat dataa , GLfloat datab, GLfloat datac , GLfloat datad );

	void            floatbuffer_addvector2( floatbuffer_t* floatbuffer , vector2_t vector );

	void            floatbuffer_addvector22( floatbuffer_t* floatbuffer , vector2_t vectora , vector2_t vectorb );

	void            floatbuffer_addvector3( floatbuffer_t* floatbuffer , vector3_t vector );

	void            floatbuffer_setprojection( floatbuffer_t* floatbuffer , matrix4_t projection );

    #endif
