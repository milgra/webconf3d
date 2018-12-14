//
//  mtbus.h
//  swarmtest
//
//  Created by Milan Toth on 2017. 08. 30..
//  Copyright © 2017. Milan Toth. All rights reserved.
//

#ifndef mtbus_h
#define mtbus_h

#include <stdio.h>

void mtbus_init( void );
void mtbus_kill( void );
void mtbus_send( char* address , char* name, void* data );
void mtbus_subscribe( char* address , void (*onmessage)(char*, void*) );
void mtbus_unsubscribe( char* address , void (*onmessage)(char*, void*) );

#endif /* mtbus_h */
