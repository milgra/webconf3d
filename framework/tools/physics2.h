

    #ifndef collision_h
    #define collision_h


    #include <stdio.h>
    #include <math.h>
    #include "math2.h"
    #include "../core/mtvec.h"


	typedef struct _mass2_t mass2_t;
	struct _mass2_t
	{
		vector2_t trans;
		vector2_t basis;
        
        float weight;
		float radius;
        float elasticity;
        
        mtvec_t* segmentgroups;
	};

    mass2_t*        mass2_alloc( vector2_t position , float radius , float weight , float elasticity );


	typedef struct _dguard2_t dguard2_t;
	struct _dguard2_t
	{
		mass2_t* mass_a;
		mass2_t* mass_b;
        
		float distance;
        float elasticity;

        float ratio_a;
        float ratio_b;
	};

    dguard2_t*      dguard2_alloc( mass2_t* mass_a , mass2_t* mass_b , float distance , float elasticity );

    void            dguard2_resetdistance( dguard2_t* dguard );

    void            dguard2_new( dguard2_t* dguard );


    typedef struct _aguard2_t aguard2_t;
    struct _aguard2_t
    {
		mass2_t* mass_a;
		mass2_t* mass_b;
		mass2_t* mass_c;
        
        float angle_min;
        float angle_max;
        
        float ratio_a;
        float ratio_c;
    };

    aguard2_t*      aguard2_alloc( mass2_t* mass_a , mass2_t* mass_b , mass2_t* mass_c , float angle_min , float angle_max );

    void            aguard2_new( aguard2_t* guard );


	typedef struct _surfaces_t surfaces_t;
	struct _surfaces_t
	{
        mtvec_t** strips;
        mtvec_t* groups;
        
        uint32_t size;
        float stripwidth;
	};

    surfaces_t*     surfaces_alloc( uint32_t size , float stripwidth );

    void            surfaces_reset( surfaces_t* surfaces );

    void            surfaces_addsegment( surfaces_t* surfaces , segment2_t* segment );

    void            surfaces_segments_for_mtvec_and_threshold( surfaces_t* surfaces , vector2_t trans , vector2_t basis , float threshold );


	typedef struct _physics2_collision_t physics2_collision_t;
    struct _physics2_collision_t
    {
        int count;
		vector2_t isps[10];
		segment2_t segments[10];
    };

    void            physics2_set_gravity( mtvec_t* masses , vector2_t gravity );

    void            physics2_set_distances( mtvec_t* dguards );

    void            physics2_set_angles( mtvec_t* aguards );

    void            physics2_set_positions( mtvec_t* masses , surfaces_t* surfaces );

    void            physics2_new_mass_position( mass2_t* mass , surfaces_t* surfaces );

    void            physics2_collect_intersecting_surfaces( surfaces_t* surfaces , vector2_t trans , vector2_t basis , float radius , physics2_collision_t* collision );
    
    void            physics2_collect_intersecting_and_nearby_surfaces( surfaces_t* surfaces , vector2_t trans , vector2_t basis , float radius , physics2_collision_t* collision );


    #endif
