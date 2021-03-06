

    #include "mtpipe.h"

    /* create pipe */

    mtpipe_t* mtpipe_alloc( uint32_t size )
    {
        mtpipe_t* boxes = mtmem_calloc( sizeof( mtpipe_t ), mtpipe_dealloc );
        
        boxes->flags = mtmem_calloc( sizeof(char) * size , NULL );
        boxes->boxes = mtmem_calloc( sizeof(void*) * size , NULL );
        boxes->size = size;
        boxes->read_index = 0;
        boxes->write_index = 0;
        
        return boxes;
    }

    /* destructor */
    
    void mtpipe_dealloc( void* pointer )
    {
        assert( pointer != NULL );
        
        mtpipe_t* boxes = pointer;
        
        mtmem_release( boxes->flags );
        mtmem_release( boxes->boxes );
    }
    
    /* send data to other thread */
    
    char mtpipe_send( mtpipe_t* boxes , void* data )
    {
        assert( boxes != NULL );
        assert( data != NULL );
        
        // wait for the box to get empty
        
        if ( boxes->flags[ boxes->write_index ] == 0 )
        {
            boxes->boxes[ boxes->write_index ] = data;
            
            // set flag, it doesn't have to be atomic, only the last bit counts
            
            boxes->flags[ boxes->write_index ] = 1;

            // increment write index, doesn't have to be atomic, this thread uses it only
            
            boxes->write_index += 1;
            if ( boxes->write_index == boxes->size ) boxes->write_index = 0;
            
            return 1;
        }
        
        return 0;
    }
    
    /* receive data from other thread */
    
    void* mtpipe_recv( mtpipe_t* boxes )
    {
        assert( boxes != NULL );
        
        if ( boxes->flags[ boxes->read_index ] == 1 )
        {
            void* result = boxes->boxes[ boxes->read_index ];
			
            // empty box
            
            boxes->boxes[ boxes->read_index ] = NULL;
            
            // set flag, it doesn't have to be atomic, only the last bit counts
            
            boxes->flags[ boxes->read_index ] = 0;
            
            // increment read index, it doesn't have to be atomic, this thread
            
            boxes->read_index += 1;
            if ( boxes->read_index == boxes->size ) boxes->read_index = 0;

            return result;
        }
        
        return NULL;
    }
    
    //
    //  TEST
    //

    #define kBoxesTestThreads 10
    
    void send_test( mtpipe_t* boxes )
    {
        uint32_t counter = 0;
        while ( 1 ) 
        {
            uint32_t* number = mtmem_calloc(sizeof(uint32_t), NULL);
            *number = counter;
            char success = mtpipe_send( boxes , number );
            if ( success == 0 ) mtmem_release( number );
            else counter += 1;
            if ( counter == UINT32_MAX - 1 ) counter = 0;
//            struct timespec time;
//            time.tv_sec = 0;
//            time.tv_nsec = rand() % 100000;
//            nanosleep(&time , (struct timespec *)NULL);            
        }
    }
    
    void recv_test( mtpipe_t* boxes )
    {
        uint32_t last = 0;
        while ( 1 ) 
        {
            uint32_t* number = mtpipe_recv( boxes );
            if ( number != NULL )
            {
                if ( *number != last ) printf( "index error!!!" );
                mtmem_release( number );
                last += 1;
                if ( last == UINT32_MAX - 1 ) last = 0;
                if ( last % 100000 == 0 ) printf( "%x OK %u %u" , ( uint32_t ) boxes , last , UINT32_MAX );
//                struct timespec time;
//                time.tv_sec = 0;
//                time.tv_nsec = rand() % 100000;
//                nanosleep(&time , (struct timespec *)NULL);            
            }
        }    
    }
    
    mtpipe_t** testarray;
    
    void mtpipe_test( )
    {
        testarray = mtmem_calloc( sizeof( mtpipe_t ) * kBoxesTestThreads , NULL );
        
        for ( int index = 0 ; index < kBoxesTestThreads ; index++ )
        {
            testarray[ index ] = mtpipe_alloc(100);
            pthread_t thread;
            pthread_create( &thread, NULL, (void*) send_test , testarray[ index ] );
            pthread_create( &thread, NULL, (void*) recv_test , testarray[ index ] );
        }
    }
    
    
    
    
