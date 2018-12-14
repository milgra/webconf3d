//
//  trafoanimator.h
//  conference
//
//  Created by milgra on 15/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

#ifndef trafoanimator_h
#define trafoanimator_h

	#include <stdio.h>
	#include "../framework/tools/math3.h"
	#include "../framework/core/mtmem.h"

    typedef struct _trafoanimator_t trafoanimator_t;
    struct _trafoanimator_t
    {
		vector3_t trans_actual;
		vector3_t scale_actual;
		vector3_t angle_actual;

		vector3_t trans_final;
		vector3_t scale_final;
		vector3_t angle_final;
		
		uint32_t counter;
	};

    trafoanimator_t* trafoanimator_alloc(  );
    void trafoanimator_dealloc( void* pointer );
	int trafoanimator_update( trafoanimator_t* animator );

#endif /* trafoanimator_h */
