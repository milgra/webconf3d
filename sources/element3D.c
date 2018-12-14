//
//  element.c
//  conference
//
//  Created by milgra on 12/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

	#include "element3D.h"
	#include "../framework/core/mtbmp.h"
	#include "../framework/core/mtcstr.h"

    element3D_t* element3D_alloc( char* name, float width , float height , vector2_t* coords , int texture )
	{
        element3D_t* element = mtmem_calloc( sizeof( element3D_t ) , element3D_dealloc );

		element->quad = quad3D_alloc( width , height , matrix4_defaultidentity() , coords , texture );
		element->name = mtcstr_fromcstring( name );
		
		printf( "CREATING ELEMENT, TEXTURE %i %i\n" , texture , element->quad->texture );
	
		return element;
	}
	
    void element3D_dealloc( void* pointer )
	{
		element3D_t* element = pointer;
		
		mtmem_release( element->quad );
		mtmem_release( element->name );
	}

    void element3D_resettransformations( element3D_t* element )
	{
		vector3_t trans = {0};
		vector3_t angle = {0};
		element->trans = trans;
		element->angle = angle;
		element->quad->matrix = matrix4_defaultidentity();
	}

    void element3D_translate( element3D_t* element , float x , float y , float z )
	{
		element->trans.x += x;
		element->trans.y += y;
		element->trans.z += z;

		matrix4_t trans = matrix4_defaulttranslation( x , y , z );
		element->quad->matrix = matrix4_multiply( trans , element->quad->matrix );
	}
	
    void element3D_rotate( element3D_t* element , float x , float y , float z )
	{
		element->angle.x += x;
		element->angle.y += y;
		element->angle.z += z;
		
		matrix4_t trans = matrix4_defaultrotation(x,y,z );
		element->quad->matrix = matrix4_multiply( trans , element->quad->matrix );
	}

    void element3D_translate_central( element3D_t* element , float x , float y , float z )
	{
		element->trans.x += x;
		element->trans.y += y;
		element->trans.z += z;
		
		matrix4_t pre = matrix4_defaulttranslation( -element->quad->width / 2.0 , -element->quad->height / 2.0 , 0.0 );
		matrix4_t pos = matrix4_defaulttranslation( element->quad->width / 2.0 , element->quad->height / 2.0 , 0.0 );
		matrix4_t trans = matrix4_defaulttranslation( x , y , z );
		pre = matrix4_multiply( trans , pre );
		pre = matrix4_multiply( pos , pre );
		element->quad->matrix = matrix4_multiply( pre , element->quad->matrix );
	}

    void element3D_rotate_central( element3D_t* element , float x , float y , float z )
	{
		element->angle.x += x;
		element->angle.y += y;
		element->angle.z += z;
		
		matrix4_t pre = matrix4_defaulttranslation( -element->quad->width / 2.0 , -element->quad->height / 2.0 , 0.0 );
		matrix4_t pos = matrix4_defaulttranslation( element->quad->width / 2.0 , element->quad->height / 2.0 , 0.0 );
		matrix4_t rot = matrix4_defaultrotation( x, y, z );
		pre = matrix4_multiply( rot , pre );
		pre = matrix4_multiply( pos , pre );
		element->quad->matrix = matrix4_multiply( pre , element->quad->matrix );
	}
	
    void element3D_collect_vertexes( element3D_t* element , floatbuffer_t* buffer )
	{
		quad3D_collect_vertexes( element->quad , buffer );
		
		// TODO texture info should be put here and removed from quad
	}
	
	
	
