//
//  element.h
//  conference
//
//  Created by milgra on 12/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

#ifndef element3D_h
#define element3D_h

	#include <stdio.h>
	#include "quad3D.h"
    #include "math3.h"
    #include "floatbuffer.h"

    typedef struct _element3D_t element3D_t;
    struct _element3D_t
    {
		char* name;
		quad3D_t* quad;
		vector3_t trans;
		vector3_t angle;
	};

    element3D_t* element3D_alloc( char* name, float width , float height , vector2_t* coords , int texture );
    void element3D_dealloc( void* view );
    void element3D_translate( element3D_t* element , float x , float y , float z );
    void element3D_translate_central( element3D_t* element , float x , float y , float z );
    void element3D_rotate( element3D_t* element , float x , float y , float z );
    void element3D_rotate_central( element3D_t* element , float x , float y , float z );
    void element3D_collect_vertexes( element3D_t* element , floatbuffer_t* buffer );
    void element3D_resettransformations( element3D_t* element );

#endif /* element3D_h */
