

    #ifndef __c36__corefile__
    #define __c36__corefile__


	#include <stdio.h>
	#include <stdlib.h>
	#include "mtmap.h"
	#include "mtstr.h"


	#define kTypeMap 1
	#define kTypeString 3
	#define kTypeVector 2


	typedef struct type_container_t type_container_t;
	struct type_container_t
	{
		char  type;
		void* data;
	};


	void        mtfile_writetofile( mtmap_t* map , char* path );

	mtmap_t*    mtfile_readfile( char* path );


	void        mtfile_appendstringtofile( mtstr_t* string , FILE* file_pointer );

	void        mtfile_appendvectortofile( mtvec_t* vector , FILE* file_pointer );

	void        mtfile_appendmaptofile( mtmap_t* map , FILE* file_pointer );


	mtmap_t*    mtfile_readmap( FILE* file_pointer );

	mtstr_t*    mtfile_readstring( FILE* file_pointer );

	mtvec_t*    mtfile_readvector( FILE* file_pointer );

	mtvec_t*    mtfile_readlines( char* path );


	type_container_t* mtfile_defaultcontainer( char type , void* data );


    #endif
