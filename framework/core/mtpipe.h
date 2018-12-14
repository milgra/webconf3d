

    #ifndef mtpipe_h
    #define mtpipe_h


    #include <time.h>
    #include <stdio.h>
    #include <assert.h>
    #include <pthread.h>
    #include "mtmem.h"
    

    typedef struct mtpipe_t mtpipe_t;
    struct mtpipe_t
    {
        char* flags;
        void** boxes;
        
        uint32_t size;
        uint32_t read_index;
        uint32_t write_index;
    };
    
    mtpipe_t*   mtpipe_alloc( uint32_t size );

    void        mtpipe_dealloc( void* pointer );

    char        mtpipe_send( mtpipe_t* boxes , void* data );

    void*       mtpipe_recv( mtpipe_t* boxes );
    
    void        mtpipe_test(void);


    #endif
