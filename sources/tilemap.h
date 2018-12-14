//
//  tilemtmap.h
//  KineticUI
//
//  Created by Milan Toth on 05/10/14.
//  Copyright (c) 2014 Milan Toth. All rights reserved.
//

    #ifndef __KineticUI__tilemap__
    #define __KineticUI__tilemap__


    #include "../framework/tools/math2.h"
    #include "../framework/core/mtmap.h"
    #include "../framework/core/mtbmp.h"


    typedef struct _tilemap_t tilemap_t;
    struct _tilemap_t
    {
        mtmap_t* posmap;
        mtbmp_t* bitmap;
        uint8_t updated;
        uint32_t north_height;
        uint32_t north_x;
        uint32_t north_y;
		uint32_t width;
		uint32_t height;
    };


    tilemap_t* tilemap_alloc( uint32_t width , uint32_t height );
	void tilemap_dealloc( void* tilemap );
    void tilemap_reset( tilemap_t* tilemap );
    vector2_t* tilemap_get( tilemap_t* tilemap , char* name );
    vector2_t* tilemap_put( tilemap_t* tilemap , char* name , mtbmp_t* tile , int inset );
    void tilemap_update( tilemap_t* tilemap , char* name , mtbmp_t* tile , int inset );
    vector2_t* tilemap_get_color( tilemap_t* tilemap , uint32_t color );


    #endif /* defined(__KineticUI__tilemap__) */
