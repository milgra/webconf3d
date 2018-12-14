

    #ifndef __c36__mtstr_
    #define __c36__mtstr_


    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
	#include <stdarg.h>
	#include "mtvec.h"
	#include "mtmap.h"

    typedef struct _mtstr_t mtstr_t;
    struct _mtstr_t
    {
        uint32_t  length;
        uint32_t  length_real;
        uint32_t  length_bytes;
        uint32_t* codepoints;
    };

    mtstr_t*	mtstr_alloc(void);

    void		mtstr_dealloc( void* string );

	void		mtstr_reset( mtstr_t* string );

	mtstr_t*    mtstr_fromformat( char* format , ... );

	mtstr_t*	mtstr_frombytes( char* bytes );

	mtstr_t*	mtstr_fromstring( mtstr_t* string );

    mtstr_t*    mtstr_substring( mtstr_t* string , int start , int end );

    mtstr_t*    mtstr_replace( mtstr_t* string , mtstr_t* newstring , int start , int end );

    mtstr_t*    mtstr_compactemojis( mtstr_t* string );

    void		mtstr_addstring( mtstr_t* mtstra , mtstr_t* mtstrb );

    void		mtstr_addbytearray( mtstr_t* string , char* bytearray );

    void		mtstr_addcodepoint( mtstr_t* string , uint32_t codepoint );

	void		mtstr_removecodepointatindex( mtstr_t* string , uint32_t index );

	void		mtstr_removecodepointsinrange( mtstr_t* string , uint32_t start , uint32_t end );

	void		mtstr_replacecodepoints( mtstr_t* string , uint32_t oldcp , uint32_t newcp );

	int8_t      mtstr_compare( mtstr_t* mtstra , mtstr_t* mtstrb );

	int         mtstr_intvalue( mtstr_t* string );

	float       mtstr_floatvalue( mtstr_t* string );

	uint32_t    mtstr_unsignedvalue( mtstr_t* string );

	mtvec_t*	mtstr_split( mtstr_t* string , char character );

	mtmap_t*	mtstr_tokenize( mtstr_t* descriptor );

	uint32_t    mtstr_find( mtstr_t* string , mtstr_t* substring , uint32_t from );

	char*		mtstr_bytes( mtstr_t* string );


    #endif
