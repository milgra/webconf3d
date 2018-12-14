//
//  renderer3D.c
//  conference
//
//  Created by milgra on 11/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

	#include "renderer3D.h"


	const char* vertex_point_source3D =
	#if defined(IOS) || defined(BRY) || defined(ASMJS)
		"attribute vec4 position;\n"
		"attribute vec2 texcoord;\n"
		"varying highp vec2 texcoordv;\n"
		"uniform mat4 projection;\n"
		"void main ( )\n"
		"{\n"
			"gl_Position = projection * position;\n"
			"texcoordv = texcoord;"
		"}\n";
	#else
		"attribute vec4 position;\n"
		"attribute vec2 texcoord;\n"
		"varying vec2 texcoordv;\n"
		"uniform mat4 projection;\n"
		"void main ( )\n"
		"{\n"
			"gl_Position = projection * position;\n"
			"texcoordv = texcoord;"
		"}\n";
	#endif

	const char* fragment_point_source3D =
	#if defined(IOS) || defined(BRY) || defined(ASMJS)
		"varying mediump vec2 texcoordv;\n"
		"uniform sampler2D texture_main;\n"
		"void main( )\n"
		"{\n"
		"	gl_FragColor = texture2D( texture_main , texcoordv , 0.0 );\n"
		"}\n";
	#else
		"varying vec2 texcoordv;\n"
		"uniform sampler2D texture_main;\n"
		"void main( )\n"
		"{\n"
		"	gl_FragColor = texture2D( texture_main , texcoordv , 0.0 );\n"
		"}\n";
	#endif

    // creates shader

    renderer3D_t* renderer3D_alloc( float width , float height , float scale )
    {
		// create and use shader
        char* uniforms[ ]   = { "2" , "projection" , "texture_main" };
        char* attributes[ ] = { "2" , "position" , "texcoord" };

		renderer3D_t* renderer = mtmem_calloc( sizeof( renderer3D_t ) , renderer3D_destruct );

        renderer->shader = ogl_shader_create( vertex_point_source3D , fragment_point_source3D , uniforms , attributes , renderer->locations );
		renderer->buffer = floatbuffer_alloc( );
		renderer->needsupload = 1;

		ogl_vertexbuffer( &renderer->vbuffer );
		
		glBindBuffer( GL_ARRAY_BUFFER , renderer->vbuffer );
		ogl_enableva_f3f2();
		
		glUseProgram( renderer->shader );

        glEnable( GL_BLEND );
        glEnable( GL_TEXTURE_2D );
        glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

        glViewport( 0.0 , 0.0 , width , height );
        glClearColor( 0.9 , 0.9 , 0.8 , 1.0 );
        glClear( GL_COLOR_BUFFER_BIT );

        glActiveTexture( GL_TEXTURE0 );
		glUniform1i( renderer->locations[ 1 ] , 0 );

		return renderer;
    }

	/* deletes renderer */

	void renderer3D_destruct( void* pointer )
	{
        renderer3D_t* renderer = pointer;
		ogl_shader_delete( renderer->shader );
		glDeleteBuffers( 1 , &renderer->vbuffer );
	}

	/* resize */

	void renderer3D_resize( renderer3D_t* renderer , float width , float height )
	{
        glViewport( 0.0 , 0.0 , width , height );
		renderer->width = width;
		renderer->height = height;
	}

	/* update matrix */

    void renderer3D_updateprojectionmatrix( renderer3D_t* renderer , matrix4_t matrix )
    {
		matrix4array_t proj_matrix = { .matrix = matrix };
		glUniformMatrix4fv( renderer->locations[ 0 ] , 1 , 0 , proj_matrix.array );
    }

	/* reset renderer */

	void renderer3D_reset( renderer3D_t* drawer )
	{
		floatbuffer_reset( drawer->buffer );
	
		drawer->needsupload = 1;
		drawer->actualindex = 0;
	}

	/* adds element */

	void renderer3D_addelement( renderer3D_t* drawer , quad3D_t* quad )
	{
		floatbuffer_t* buffer = drawer->buffer;
		
		if ( buffer->length == 0 )
		{
			drawer->actualtexture = quad->texture;
			drawer->groups[ drawer->actualindex ].border = 0;		
			drawer->groups[ drawer->actualindex ].texture = quad->texture;
		}
		
		/* create new group if needed */
		
		if ( quad->texture != drawer->actualtexture )
		{
			drawer->actualtexture = quad->texture;
			drawer->actualindex += 1;
			
			vertexgroup_t group;
			group.texture = quad->texture;
			group.border = buffer->length;
			
			drawer->groups[ drawer->actualindex ] = group;
			
			printf( "ADDING NEW TRANSLATION GROUP index %i border %u tex %i\n" , drawer->actualindex , buffer->length , quad->texture );
		}

		vector4_t ul = vector4_init( 0.0, 0.0, 0.0 , 1.0 );
		vector4_t ur = vector4_init( quad->width, 0.0, 0.0 , 1.0 );
		vector4_t bl = vector4_init( 0.0, quad->height, 0.0 , 1.0 );
		vector4_t br = vector4_init( quad->width, quad->height, 0.0, 1.0 );

		vector4_t rul = matrix4_multiply_vector4( quad->matrix , ul );
		vector4_t rur = matrix4_multiply_vector4( quad->matrix , ur );
		vector4_t rbl = matrix4_multiply_vector4( quad->matrix , bl );
		vector4_t rbr = matrix4_multiply_vector4( quad->matrix , br );
		
		floatbuffer_addvector3( buffer , vector3_init( rul.x, rul.y, rul.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[0] );
		floatbuffer_addvector3( buffer , vector3_init( rbl.x, rbl.y, rbl.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[2] );
		floatbuffer_addvector3( buffer , vector3_init( rbr.x, rbr.y, rbr.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[3] );

		floatbuffer_addvector3( buffer , vector3_init( rul.x, rul.y, rul.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[0] );
		floatbuffer_addvector3( buffer , vector3_init( rbr.x, rbr.y, rbr.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[3] );
		floatbuffer_addvector3( buffer , vector3_init( rur.x, rur.y, rur.z ) );
		floatbuffer_addvector2( buffer , quad->texcoords[1] );
		
		/* update last group border */

		drawer->groups[ drawer->actualindex ].border = buffer->length;
	}

	/* draw groups */
	
	void renderer3D_draw( renderer3D_t* drawer )
	{
		glClear( GL_COLOR_BUFFER_BIT );

		if ( drawer->needsupload == 1 )
		{
			glBufferData( GL_ARRAY_BUFFER , sizeof(GLfloat) * drawer->buffer->length , drawer->buffer->data , GL_STATIC_DRAW );
			drawer->needsupload = 0;
			// printf( "UPLAODED %lu BYTES\n" , sizeof(GLfloat) * drawer->buffer->length );
		}
		
		GLint lastborder = 0;
		for ( int index = 0 ; index < drawer->actualindex + 1; index++ )
		{
			vertexgroup_t* group = &drawer->groups[ index ];
			glBindTexture(GL_TEXTURE_2D, group->texture );
			glDrawArrays( GL_TRIANGLES , lastborder / 5 , ( group->border - lastborder ) / 5 );
			// printf( "DRAWING FROM %i SIZE %i TRANS %i\n" , lastborder , ( group->border - lastborder ) / 5 , group->texture );
			lastborder = group->border;
		}
	}
