#include "mtbus.h"
#include "mtmap.h"
#include "mtvec.h"
#include "mtmem.h"

typedef struct _mtbus_fp
{
    void (*onmessage)(char*, void*);
} mtbus_fp;

mtmap_t* mtbus_subscribers = NULL;

void mtbus_init( )
{
    if ( mtbus_subscribers == NULL )
    {
        mtbus_subscribers = mtmap_alloc();
    }
}

void mtbus_kill( )
{
    mtmem_release( mtbus_subscribers );
    mtbus_subscribers = NULL;
}

// kene immediate option, vagy runloop elejen, threadsafeness?

void mtbus_send( char* address , char* name, void* data )
{
    mtvec_t* listeners = mtmap_get( mtbus_subscribers , address );
    if ( listeners != NULL )
    {
        for ( int index = 0 ; index < listeners->length ; index++ )
        {
            mtbus_fp* fp = listeners->data[ index ];
            fp->onmessage( name , data );
        }
    }
}

void mtbus_subscribe( char* address , void (*onmessage)(char*, void*) )
{
    mtvec_t* listeners = mtmap_get( mtbus_subscribers , address );
    if ( listeners == NULL )
    {
        listeners = mtvec_alloc();
        mtmap_put( mtbus_subscribers , address , listeners );
    }
    
    mtbus_fp* data = mtmem_alloc( sizeof( mtbus_fp ), NULL );
    data->onmessage = onmessage;
    mtvec_adddata( listeners , data );
}

void mtbus_unsubscribe( char* address , void (*onmessage)(char*, void*) )
{
    mtvec_t* listeners = mtmap_get( mtbus_subscribers , address );
    if ( listeners != NULL )
    {
        mtvec_removedata( listeners , onmessage );
    }
}
