

    #ifndef cmd_h
    #define cmd_h


    #include <stdio.h>
    #include "../core/mtvec.h"

    typedef struct _cmd_t cmd_t;
	struct _cmd_t
	{
		char*       name;
		void*       data;
		void*       args;
        uint32_t    time;
	};

	cmd_t*  cmd_alloc( char* name , void* view , void* arguments );
    
	void    cmd_dealloc( void* cmd );

    typedef struct _cmdqueue_t cmdqueue_t;
	struct _cmdqueue_t
	{
        mtvec_t* commands;
        mtvec_t* delayed;
        char is_iterating;
	};

    cmdqueue_t* cmdqueue_alloc(void);

	void        cmdqueue_dealloc( void* pointer );

	void        cmdqueue_setiteration( cmdqueue_t* queue );

	void        cmdqueue_add( cmdqueue_t* queue , char* name , void* data , void* args );

	void        cmdqueue_addcmd( cmdqueue_t* queue , cmd_t* cmd );

	void        cmdqueue_addonce( cmdqueue_t* queue , char* name , void* data , void* args );

    void        cmdqueue_delay( cmdqueue_t* queue , char* name , void* data , void* args , uint32_t time );

    void        cmdqueue_timer( cmdqueue_t* timer , uint32_t time );

	void        cmdqueue_reset( cmdqueue_t* queue );


    #endif
