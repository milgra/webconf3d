

	#include "settings.h"
	#include "../core/mtmem.h"
	#include "../core/mtcstr.h"

	void settings_dealloc( void* pointer );


    /* creates settings */

	settings_t* settings_alloc( char* path , char* name )
	{
		settings_t* result = mtmem_calloc( sizeof( settings_t ) , NULL );
        
		result->path = mtcstr_fromformat( "%s/%s" , path , name , settings_dealloc , NULL );
		result->map = mtfile_readfile( result->path );
        result->changed = 0;
		
		if ( result->map == NULL )
		{
			result->map = mtmap_alloc( );
			mtfile_writetofile( result->map , path );
		}

		return result;	
	}

    /* deletes settings */

	void settings_dealloc( void* pointer )
    {
        settings_t* settings = pointer;
        
        mtmem_release( settings->map );
        mtmem_release( settings->path );
    }

    /* resets settings */

	void settings_reset( settings_t* settings )
	{
		mtmem_release( settings->map );
		settings->map = mtmap_alloc( );
		remove( settings->path );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}

    /* deletes value and key */

	void settings_del( settings_t* settings , char* key  )
	{
		mtmap_del( settings->map , key );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}


    /* sets type container for given key */

	void settings_set( settings_t* settings , char* key , type_container_t* value )
	{
		mtmap_put( settings->map , key , value );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}

    /* sets integer for given key */

	void settings_setint( settings_t* settings , char* key , int value )
	{
        char numstring[64] = {0};
        snprintf( numstring , 64 , "%i" , value );
		mtstr_t* string = mtstr_frombytes( numstring );
		type_container_t* container = mtfile_defaultcontainer( kTypeString , string );
		mtmap_put( settings->map , key , container );
		mtmem_release( string );
		mtmem_release( container );
		mtfile_writetofile( settings->map , settings->path );	
        settings->changed = 1;
	}

    /* sets float for given key */

	void settings_setfloat( settings_t* settings , char* key , float value )
	{
        char numstring[64] = {0};
        snprintf( numstring , 64 , "%.4f" , value );
		mtstr_t* string = mtstr_frombytes( numstring );
		type_container_t* container = mtfile_defaultcontainer( kTypeString , string );
		mtmap_put( settings->map , key , container );
		mtmem_release( string );
		mtmem_release( container );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}

    /* sets string for given key */

	void settings_setstring( settings_t* settings , char* key , mtstr_t* value )
	{
        type_container_t* container = mtfile_defaultcontainer( kTypeString , value );
		mtmap_put( settings->map , key , container );
        mtmem_release( container );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}

    /* sets unsigned int for given key */

	void settings_setunsigned( settings_t* settings , char* key , uint32_t value )
	{
        char numstring[64] = {0};
        snprintf( numstring , 64 , "%lu" , ( unsigned long ) value );
		mtstr_t* string = mtstr_frombytes( numstring );
		type_container_t* container = mtfile_defaultcontainer( kTypeString , string );
		mtmap_put( settings->map , key , container );
		mtmem_release( string );
		mtmem_release( container );
		mtfile_writetofile( settings->map , settings->path );
        settings->changed = 1;
	}


    /* returns type container for given key */
    
	type_container_t* settings_get( settings_t* settings , char* key )
	{
		type_container_t* result = mtmap_get( settings->map , key );
		return result;
	}

    /* returns integer for given key */

	int settings_getint( settings_t* settings , char* key )
	{
		type_container_t* value = mtmap_get( settings->map , key );
		if ( value != NULL ) return mtstr_intvalue( value->data );
		else return 0;
	}

    /* returns float for given key */

	float settings_getfloat( settings_t* settings , char* key )
	{
		type_container_t* value = mtmap_get( settings->map , key );
		if ( value != NULL ) return mtstr_floatvalue( value->data );
		else return 0.0;
	}

    /* returns string for given key */

	mtstr_t* settings_getstring( settings_t* settings , char* key )
	{
		type_container_t* value = mtmap_get( settings->map , key );
		if ( value != NULL ) return value->data;
		else return NULL;
	}

    /* returns unsigned int for given key */

	uint32_t settings_getunsigned( settings_t* settings , char* key )
	{
		type_container_t* value = mtmap_get( settings->map , key );
		if ( value != NULL ) return ( uint32_t ) mtstr_unsignedvalue( value->data );
		else return 0;	
	}


