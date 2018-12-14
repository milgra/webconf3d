//
//  trafoanimator.c
//  conference
//
//  Created by milgra on 15/08/16.
//  Copyright © 2016 milgra. All rights reserved.
//

	#include "trafoanimator.h"

    trafoanimator_t* trafoanimator_alloc(  )
	{
		trafoanimator_t* trafo = mtmem_calloc( sizeof( trafoanimator_t ) , trafoanimator_dealloc );
		trafo->trans_actual = vector3_init(0.0, 0.0, 0.0);
		trafo->scale_actual = vector3_init(1.0, 1.0, 1.0);
		trafo->angle_actual = vector3_init(0.0, 0.0, 0.0);
		trafo->trans_final = vector3_init(0.0, 0.0, 0.0);
		trafo->scale_final = vector3_init(1.0, 1.0, 1.0);
		trafo->angle_final = vector3_init(0.0, 0.0, 0.0);
		trafo->counter = 210;
		return trafo;	
	}
	
    void trafoanimator_dealloc( void* pointer )
	{
		//trafoanimator_t* trafo = pointer;
	}
	
	int trafoanimator_update( trafoanimator_t* animator )
	{
		if ( animator->counter++ < 60 )
		{
			animator->trans_actual = vector3_add( animator->trans_actual , vector3_scale( vector3_sub( animator->trans_final , animator->trans_actual ) , 0.1 ) );
			animator->scale_actual = vector3_add( animator->scale_actual , vector3_scale( vector3_sub( animator->scale_final , animator->scale_actual ) , 0.1 ) );
			animator->angle_actual = vector3_add( animator->angle_actual , vector3_scale( vector3_sub( animator->angle_final , animator->angle_actual ) , 0.1 ) );

			if ( animator->counter == 60 )
			{
				animator->trans_actual = animator->trans_final;
				animator->scale_actual = animator->scale_final;
				animator->angle_actual = animator->angle_final;			
			}
			return 1;
		}
		return 0;
	}
