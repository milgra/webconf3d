//
//  renderer3D.h
//  conference
//
//  Created by milgra on 11/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

#ifndef renderer3D_h
#define renderer3D_h


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

	#include <time.h>
    #include <sys/time.h>
	#include "quad3D.h"
    #include "../framework/core/mtmap.h"
    #include "../framework/tools/ogl.h"
    #include "../framework/tools/math4.h"
	#include "../framework/tools/floatbuffer.h"

	typedef struct _vertexgroup_t vertexgroup_t;
	struct _vertexgroup_t
	{
		uint32_t border;
		GLint texture;
	};

	typedef struct _renderer3D_t renderer3D_t;
	struct _renderer3D_t
	{
		char needsupload;

		float width;
		float height;

		GLuint shader;
		GLuint vbuffer;
		GLint locations[2];
		GLint actualtexture;
		uint32_t actualindex;

		vertexgroup_t groups[100];
		floatbuffer_t* buffer;
	};


    renderer3D_t* renderer3D_alloc( float width , float height , float scale );
	void renderer3D_destruct( void* renderer );
	void renderer3D_resize( renderer3D_t* renderer , float width , float height );
    void renderer3D_updateprojectionmatrix( renderer3D_t* renderer , matrix4_t matrix );
	void renderer3D_addelement( renderer3D_t* drawer , quad3D_t* element );
	void renderer3D_reset( renderer3D_t* drawer );
	void renderer3D_draw( renderer3D_t* drawer );


#endif /* renderer3D_h */
