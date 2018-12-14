//
//  controller.h
//
//  Created by Milan Toth on 26/09/15.
//  Copyright (c) 2015 Milan Toth. All rights reserved.
//

#ifndef __parabola__controller__
#define __parabola__controller__

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include "../framework/core/mtmem.h"
	#include "../framework/ui/input.h"
	#include "quad3D.h"
    #include "tilemap.h"
	#include "../framework/tools/font.h"
	#include "../framework/ui/text.h"
	#include "../framework/core/mtbmp.h"
	#include "../framework/core/mtcstr.h"
	#include "../framework/tools/math4.h"
	#include "../framework/tools/image.h"
	#include "element3D.h"
	#include "renderer3D.h"
	#include "../framework/tools/floatbuffer.h"
	#include "trafoanimator.h"
    #include "../framework/tools/ogl.h"
    #include <float.h>

    typedef struct _environment_t environment_t;
	struct _environment_t
	{
		float       scale;
		float       width;
		float       height;
		char        windowed;
        
		char*       path_library;
		char*       path_resources;
        
        uint32_t    defaultFrameBuffer;
        uint32_t    defaultRenderBuffer;
    };

	typedef struct _controller_t controller_t;
	struct _controller_t
	{
        environment_t env;
    
		uint64_t ticks;
		float scale;
		float width;
		float height;
		
		char windowed;
		char* path_library;
		char* path_resources;
		
		float uiwidth;
		float uiheight;
		
		float zoom;		
		float trans;
		float angle;
		
		matrix4_t pers_matrix;
		matrix4_t proj_matrix;
		
		font_t* font;
		mtvec_t* labels;
		mtvec_t* avatars;
		mtvec_t* elements;
        
		element3D_t* selected;
		element3D_t* screen;
		element3D_t* presenter;

		element3D_t* labelA;
		element3D_t* labelB;
		element3D_t* labelC;
		
		tilemap_t* tilemap;
		tilemap_t* screenmap;

		renderer3D_t* renderer;
		trafoanimator_t* animator;
		mtbmp_t* blackmap;
		
		int playhead;
		
		char* videos[10];
		
		vector2_t panvector;
		vector2_t fullcoords[4];

		GLuint textures[10];
	};

	controller_t* controller_alloc( environment_t env );
	void controller_dealloc( void* controller );
	void controller_render( controller_t* controller , char force );
	void controller_handleactions( controller_t* controller );
	void controller_input( controller_t* controller , input_t* input );
	void controller_output( input_t* input );

#endif /* defined(__parabola__controller__) */
