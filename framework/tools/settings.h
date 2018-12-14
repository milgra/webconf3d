

    #ifndef settings_h
    #define settings_h


	#include "../core/mtfile.h"
	#include "../core/mtmap.h"
	#include "../core/mtstr.h"

	typedef struct _settings_t settings_t;
	struct _settings_t
	{
		mtmap_t* map;
		char* path;
        char changed;
	};

	settings_t*         settings_alloc( char* path , char* name );

	void                settings_reset( settings_t* settings );

	void                settings_del( settings_t* settings , char* key );


	void                settings_set( settings_t* settings , char* key , type_container_t* value );

	void                settings_setint( settings_t* settings , char* key , int value );

	void                settings_setfloat( settings_t* settings , char* key , float value );

	void                settings_setstring( settings_t* settings , char* key , mtstr_t* value );

	void                settings_setunsigned( settings_t* settings , char* key , uint32_t value );


	type_container_t*   settings_get( settings_t* settings , char* key );

	int                 settings_getint( settings_t* settings , char* key );

	float               settings_getfloat( settings_t* settings , char* key );

	mtstr_t*            settings_getstring( settings_t* settings , char* key );

	uint32_t            settings_getunsigned( settings_t* settings , char* key );


    #endif
