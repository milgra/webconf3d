

    #ifndef nbsocket_h
    #define nbsocket_h


	#include <stdio.h>
	#include <unistd.h>

	typedef struct nbsocket_t nbsocket_t;
	struct nbsocket_t
	{
		int     socket;
		char    connected;
	};

	nbsocket_t* nbsocket_alloc(void);

	void        nbsocket_dealloc( void* pointer );

	int         nbsocket_connect( nbsocket_t* nbsocket , char* host , int port );

	void        nbsocket_close( nbsocket_t* nbsocket );

	int         nbsocket_timer( nbsocket_t* mqtt , char* readbuffer , ssize_t* readlength , char* sendbuffer , ssize_t* sendlength );


    #endif
