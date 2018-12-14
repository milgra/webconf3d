

    #include "floatbuffer.h"


	/* alloc floatbuffer */

    floatbuffer_t* floatbuffer_alloc( )
    {
        floatbuffer_t* floatbuffer = mtmem_calloc( sizeof( floatbuffer_t ) , floatbuffer_dealloc );
        
		floatbuffer->data = mtmem_calloc( sizeof( GLfloat ) * 10 , NULL );
		floatbuffer->length = 0;
		floatbuffer->length_real = 10;
        
        return floatbuffer;
    }

	/* dealloc floatbuffer */

	void floatbuffer_dealloc( void* pointer )
	{
        floatbuffer_t* floatbuffer = pointer;
        mtmem_release( floatbuffer->data );
	}

	/* resets floatbuffer */

	void floatbuffer_reset( floatbuffer_t* floatbuffer )
	{
		floatbuffer->length = 0;
	}

	/* internal use, expands internal storage size */

	void floatbuffer_expand( floatbuffer_t* floatbuffer )
	{
        assert( floatbuffer->length_real < UINT32_MAX / 2 );
        floatbuffer->length_real *= 2;
        floatbuffer->data = mtmem_realloc( floatbuffer->data , sizeof( void* ) * floatbuffer->length_real );
	}

	/* adds single float to buffer */

	void floatbuffer_add( floatbuffer_t* floatbuffer , GLfloat value )
	{
		if ( floatbuffer->length == floatbuffer->length_real ) floatbuffer_expand( floatbuffer );
		floatbuffer->data[ floatbuffer->length ] = value;
		floatbuffer->length += 1;
		floatbuffer->changed = 1;
	}

	/* adds four float to buffer */

	void floatbuffer_add4( floatbuffer_t* floatbuffer , GLfloat dataa , GLfloat datab, GLfloat datac , GLfloat datad )
    {
		if ( floatbuffer->length + 4 >= floatbuffer->length_real ) floatbuffer_expand( floatbuffer );
		floatbuffer->data[ floatbuffer->length ] = dataa;
		floatbuffer->data[ floatbuffer->length + 1 ] = datab;
		floatbuffer->data[ floatbuffer->length + 2 ] = datac;
		floatbuffer->data[ floatbuffer->length + 3 ] = datad;
		floatbuffer->length += 4;
		floatbuffer->changed = 1;
    }

	/* adds vector2_t to floatbuffer */

	void floatbuffer_addvector2( floatbuffer_t* floatbuffer , vector2_t vector )
    {
		if ( floatbuffer->length + 2 >= floatbuffer->length_real ) floatbuffer_expand( floatbuffer );
		floatbuffer->data[ floatbuffer->length ] = vector.x;
		floatbuffer->data[ floatbuffer->length + 1 ] = vector.y;
		floatbuffer->length += 2;
		floatbuffer->changed = 1;
    }

	/* adds 2 vector2_t to floatbuffer */

	void floatbuffer_addvector22( floatbuffer_t* floatbuffer , vector2_t vectora , vector2_t vectorb )
    {
		if ( floatbuffer->length + 4 >= floatbuffer->length_real ) floatbuffer_expand( floatbuffer );
		floatbuffer->data[ floatbuffer->length ] = vectora.x;
		floatbuffer->data[ floatbuffer->length + 1 ] = vectora.y;
		floatbuffer->data[ floatbuffer->length + 2] = vectorb.x;
		floatbuffer->data[ floatbuffer->length + 3 ] = vectorb.y;
		floatbuffer->length += 4;
		floatbuffer->changed = 1;
    }

	/* adds vector3_t to floatbuffer */

	void floatbuffer_addvector3( floatbuffer_t* floatbuffer , vector3_t vector )
    {
		if ( floatbuffer->length + 3 >= floatbuffer->length_real ) floatbuffer_expand( floatbuffer );
		floatbuffer->data[ floatbuffer->length ] = vector.x;
		floatbuffer->data[ floatbuffer->length + 1 ] = vector.y;
		floatbuffer->data[ floatbuffer->length + 2 ] = vector.z;
		floatbuffer->length += 3;
		floatbuffer->changed = 1;
    }

	void floatbuffer_setprojection( floatbuffer_t* floatbuffer , matrix4_t projection )
    {
		floatbuffer->projection = projection;
	}
