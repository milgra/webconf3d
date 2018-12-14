//
//  json.c
//  common
//
//  Created by Milan Toth on 2016. 12. 13..
//  Copyright © 2016. Tamas Korodi. All rights reserved.
//

    #include "json.h"
    #include <string.h>
    #include "../core/mtmem.h"
    #include "../core/mtstr.h"
    #include "../core/mtcstr.h"


    /* parses c string into an mtmap */

    mtmap_t* json_parse( char* string )
    {
        mtmap_t* result = mtmap_alloc( );

        jsmn_parser parser;
        jsmntok_t tokens[ 128 ] = { 0 }; /* We expect no more than 128 tokens */
        mtmap_t* objects[ 128 ] = { 0 };
        objects[ 0 ] = result;

        jsmn_init( &parser );
        
        int count = jsmn_parse( &parser, string, strlen( string ), tokens, sizeof( tokens ) / sizeof( tokens[ 0 ] ) );
        
        if ( count < 1 ) return result;
        if ( tokens[0].type != JSMN_OBJECT ) return result;

        for (int index = 1; index < count; index++) 
        {
            jsmntok_t tok = tokens[ index ];

            if ( tok.type == JSMN_ARRAY )
            {
                jsmntok_t parent = tokens[ tok.parent ];
                char* key = strndup( string + parent.start , parent.end - parent.start );
                char* value = strndup( string + tok.start , tok.end - tok.start );
                char* managedstring = mtcstr_fromcstring( value );
                mtmap_t* parentmap = objects[ parent.parent ];
                mtmap_put( parentmap , key , managedstring );
                
                free( key );
                free ( value );
                mtmem_release( managedstring );
            }
            else if ( tok.type == JSMN_OBJECT )
            {
                jsmntok_t parent = tokens[ tok.parent ];
                char* key = strndup( string + parent.start , parent.end - parent.start );
                
                mtmap_t* map = mtmap_alloc( );
                mtmap_t* parentmap = objects[ parent.parent ];
                mtmap_put( parentmap , key , map );
                
                objects[ index ] = map;
                
                mtmem_release( map );
                free( key );
            }
            else
            {
                jsmntok_t parent = tokens[ tok.parent ];
                
                if ( parent.type != JSMN_OBJECT && parent.type != JSMN_ARRAY )
                {
                    char* key = strndup( string + parent.start , parent.end - parent.start );
                    char* value = strndup( string + tok.start , tok.end - tok.start );

                    char* mngstring = mtcstr_fromcstring( value );
                    
                    mtmap_t* parentmap = objects[ parent.parent ];
                    mtmap_put( parentmap , key , mngstring );
                    
                    free( key );
                    free( value );
                    mtmem_release( mngstring );
                }
            }	
        }
        return result;
    }
