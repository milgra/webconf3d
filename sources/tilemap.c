//
//  tilemap.c
//  KineticUI
//
//  Created by Milan Toth on 05/10/14.
//  Copyright (c) 2014 Milan Toth. All rights reserved.
//

    #include "tilemap.h"
    #include "../framework/core/mtmem.h"
    #include "../framework/core/mtbmp.h"


    // creates tile map hashmap

    tilemap_t* tilemap_alloc( uint32_t width , uint32_t height )
    {
    
        tilemap_t* tilemap = mtmem_calloc( sizeof( tilemap_t ) , tilemap_dealloc );
        tilemap->bitmap = mtbmp_alloc( width , height );
        tilemap->posmap = mtmap_alloc( );
        tilemap->updated = 0;
        tilemap->north_height = 0;
        tilemap->north_x = 0;
        tilemap->north_y = 0;
		tilemap->width = width;
		tilemap->height = height;
        
        return tilemap;
    }

	// deletes tile map

	void tilemap_dealloc( void* pointer )
	{
        tilemap_t* tilemap = pointer;
		// delete coordinates from posmap
		mtmem_release( tilemap->posmap );
		mtmem_release( tilemap->bitmap );
	}

	// resets tilemap

    void tilemap_reset( tilemap_t* tilemap )
	{
		mtmem_release( tilemap->posmap );
		mtmem_release( tilemap->bitmap );
        
        tilemap->bitmap = mtbmp_alloc( tilemap->width , tilemap->height );
        tilemap->posmap = mtmap_alloc( );
        tilemap->north_height = 0;
        tilemap->north_x = 0;
        tilemap->north_y = 0;
        tilemap->updated = 1;
	}

    // returns tile info in tilemap

    vector2_t* tilemap_get( tilemap_t* tilemap , char* name )
    {
        vector2_t* coordinates = mtmap_get( tilemap->posmap , name );
		// if ( coordinates != NULL ) printf( "getting from tilemap %s coords %f %f" , name , coordinates->x , coordinates->y );
        return coordinates;
    }

    // puts tile info and tile into tilemap

    vector2_t* tilemap_put( tilemap_t* tilemap , char* name , mtbmp_t* tile , int inset )
    {
//        vector2_t* storedcoords = map_get( tilemap->posmap , name );
//		
//		if ( storedcoords != NULL ) return storedcoords;
        // check dimensions
    
        if ( tile->width > tilemap->bitmap->width || tile->height > tilemap->bitmap->height ) { printf( "Too big tile" ); return NULL; }
    
        if ( tilemap->north_x + tile->width >= tilemap->bitmap->width )
        {
            tilemap->north_y += tilemap->north_height + 2;
            tilemap->north_x = 0;
            tilemap->north_height = 0;
        }

        if ( tilemap->north_y + tile->height >= tilemap->bitmap->height )
		{
			printf( "Too tall tile" );
			return NULL;
		}
        
        // add tile to bitmap
        
        mtbmp_replace_from( tilemap->bitmap , tile , tilemap->north_x , tilemap->north_y );
        
        // calculate relative corner positions for textures

        float ulx = ( ( float ) tilemap->north_x ) / ( ( float ) tilemap->bitmap->width );
        float uly = ( ( float ) tilemap->north_y ) / ( ( float ) tilemap->bitmap->height );
        float urx = ( ( float ) tilemap->north_x + (float)tile->width ) / ( ( float ) tilemap->bitmap->width );
        float ury = uly;
        float llx = ulx;
        float lly = ( ( float ) tilemap->north_y + (float)tile->height ) / ( ( float ) tilemap->bitmap->height );
        float lrx = urx;
        float lry = lly;

        // printf( "%s X %u EDGE %u cx %f cy %f\n" , name , tilemap->north_x , tilemap->north_x + tile->width , ulx , urx );

        vector2_t* coordinates = mtmem_calloc( sizeof( vector2_t ) * 5 , NULL );
        coordinates[0] = vector2_init( ulx , uly );
        coordinates[1] = vector2_init( urx , ury );
        coordinates[2] = vector2_init( llx , lly );
        coordinates[3] = vector2_init( lrx , lry );
        coordinates[4] = vector2_init( tilemap->north_x , tilemap->north_y );

        // increase cursor

        tilemap->north_x += tile->width + 2;
        if ( tilemap->north_height < tile->height ) tilemap->north_height = tile->height;
        
        mtmap_put( tilemap->posmap , name , coordinates );
        tilemap->updated = 1;

        // printf( "adding to tilemap %s %i %i coords %f %f" , name , tile->width , tile->height , ulx , uly );
		
        return coordinates;
    }

    void tilemap_update( tilemap_t* tilemap , char* name , mtbmp_t* tile , int inset )
    {
        vector2_t* coordinates = mtmap_get( tilemap->posmap , name );
		if ( coordinates != NULL )
		{
			vector2_t coord = coordinates[4];
			mtbmp_replace_from( tilemap->bitmap , tile , (int)coord.x , (int)coord.y );
			tilemap->updated = 1;
		}	
	}

    // returns color tile info, adds color if needed

    vector2_t* tilemap_get_color( tilemap_t* tilemap , uint32_t color )
    {
		char* name = mtmem_calloc( sizeof( char ) * 9 , NULL );
		sprintf( name , "%x" , color );
    
        vector2_t* coords = tilemap_get( tilemap , name );
        
        if ( coords == NULL )
        {
            mtbmp_t* bitmap = mtbmp_alloc( 30 , 30 );	// needs release
            mtbmp_fill_with_color( bitmap , 0 , 0 , 30 , 30 , color );
            coords = tilemap_put( tilemap , name , bitmap , 2 );
			mtmem_release( bitmap );
        }
        
        mtmem_release( name );
        return coords;
    }


