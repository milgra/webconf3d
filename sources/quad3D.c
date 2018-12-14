//
//  quad3D.c
//  conference
//
//  Created by milgra on 11/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

	#include "quad3D.h"

    quad3D_t* quad3D_alloc( float width , float height , matrix4_t matrix, vector2_t* texcoords , int texture )
	{
        quad3D_t* quad = mtmem_calloc( sizeof( quad3D_t ) , quad3D_dealloc );
		quad->width = width;
		quad->height = height;
		quad->matrix = matrix;
		quad->texture = texture;
		quad->texcoords = texcoords;
	
		return quad;
	}

    void quad3D_dealloc( void* pointer )
	{
		//quad3D_t* quad = pointer;
	}

	vector3_t quad3D_center( quad3D_t* quad )
	{
		vector4_t ul = vector4_init( 0.0, 0.0, 0.0 , 1.0 );
		vector4_t br = vector4_init( quad->width, quad->height, 0.0, 1.0 );

		vector4_t rul = matrix4_multiply_vector4( quad->matrix , ul );
		vector4_t rbr = matrix4_multiply_vector4( quad->matrix , br );
		
		vector3_t ulv = vector3_init( rul.x , rul.y , rul.z ); 
		vector3_t brv = vector3_init( rbr.x , rbr.y , rbr.z );
		vector3_t sub = vector3_sub( brv , ulv );
		return vector3_add( ulv , vector3_scale( sub , .5 ) ); 
	}
	
	vector3_t quad3D_intersect( quad3D_t* quad , vector3_t line_a , vector3_t line_b )
	{
		vector4_t ul = vector4_init( 0.0, 0.0, 0.0 , 1.0 );
		vector4_t ur = vector4_init( quad->width, 0.0, 0.0 , 1.0 );
		vector4_t bl = vector4_init( 0.0, quad->height, 0.0 , 1.0 );

		vector4_t rul = matrix4_multiply_vector4( quad->matrix , ul );
		vector4_t rur = matrix4_multiply_vector4( quad->matrix , ur );
		vector4_t rbl = matrix4_multiply_vector4( quad->matrix , bl );
		
		return vector4_quadlineintersection( rul, rur, rbl, line_a, line_b );
	}
	
    void quad3D_collect_vertexes( quad3D_t* quad , floatbuffer_t* buffer )
	{
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
	}
