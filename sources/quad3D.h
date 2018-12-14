//
//  quad3D.h
//  conference
//
//  Created by milgra on 11/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

#ifndef quad3D_h
#define quad3D_h

	#include <stdio.h>
	#include "../framework/tools/math2.h"
	#include "../framework/tools/math4.h"
	#include "../framework/tools/floatbuffer.h"

    typedef struct _quad3D_t quad3D_t;
    struct _quad3D_t
    {
		float width;
		float height;
		matrix4_t matrix;

		int texture;
		vector2_t* texcoords;
	};

    quad3D_t* quad3D_alloc( float width , float height , matrix4_t matrix, vector2_t* texcoords , int texture );
    void quad3D_dealloc( void* view );
    void quad3D_collect_vertexes( quad3D_t* quad , floatbuffer_t* buffer );
	vector3_t quad3D_center( quad3D_t* quad );
	vector3_t quad3D_intersect( quad3D_t* quad , vector3_t line_a , vector3_t line_b );

#endif /* quad3D_h */
