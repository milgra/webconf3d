

    #ifndef C36F_GLMath_h
    #define C36F_GLMath_h

	#include <math.h>
	#include <stdio.h>
	#include <stdlib.h>
    #include <stdint.h>
    #include "../core/mtvec.h"

	typedef struct _vector2_t vector2_t;
	struct _vector2_t
	{
		float x, y;
	};

	typedef struct _segment2_t segment2_t;
	struct _segment2_t
	{
		vector2_t trans;
		vector2_t basis;
	};

	vector2_t	vector2_init( float x , float y );

	vector2_t	vector2_add( vector2_t a , vector2_t b );

	vector2_t	vector2_sub( vector2_t a , vector2_t b );

	vector2_t   vector2_scale( vector2_t vector , float ratio );

	vector2_t	vector2_resize( vector2_t a , float size );

	vector2_t   vector2_rotate( vector2_t vector , float newangle );

	vector2_t   vector2_rotate_90_left( vector2_t vector );

	vector2_t   vector2_rotate_90_right( vector2_t vector );

    vector2_t   vector2_mirror( vector2_t axis , vector2_t vector );

    vector2_t   vector2_midpoints( vector2_t pointa , vector2_t pointb );

	float		vector2_length( vector2_t a );

	float		vector2_angle_x( vector2_t a );

    float       vector2_circular_angle_between( vector2_t a , vector2_t b );

	char		vector2_equals( vector2_t a , vector2_t b );

	void		vector2_describe( vector2_t vector );

	float       vector2_longside( vector2_t a );

	vector2_t   vector2_intersect_lines( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb );

	char        vector2_point_inside_vector( vector2_t transa , vector2_t basisa , vector2_t point );

	vector2_t   vector2_intersect_vectors( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb );


	char		vector2_box_intersect( vector2_t basisa , vector2_t transa , vector2_t basisb , vector2_t transb , float extra_distance );
    float       vector2_endpoint_proximity( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb );
	vector2_t	vector2_intersect_with_proximity( vector2_t trans_a , vector2_t basis_a , vector2_t trans_b , vector2_t basis_b , float proximity );
	vector2_t   vector2_intersect_with_nearby( vector2_t trans_a , vector2_t basis_a , vector2_t trans_b , vector2_t basis_b , float proximity );
    vector2_t   vector2_triangle_with_bases( vector2_t point_a , vector2_t point_b , float segmentlength , int8_t direction );
    segment2_t  vector2_collide_and_fragment( vector2_t transa , vector2_t basisa , vector2_t transb , vector2_t basisb );


    segment2_t  segment2_init( vector2_t trans , vector2_t basis );

	#define kSquareOverlappingNone 0
	#define kSquareOverlappingInside 1
	#define kSquareOverlappingPartial 2

	typedef struct square2_t square2_t;
	struct square2_t
	{
		vector2_t origo;
		vector2_t extent;
	};

	square2_t	square_alloc( vector2_t origo , vector2_t extent );

	square2_t	square_intersect( square2_t squarea , square2_t squareb );

	uint8_t		square_checkoverlapping( square2_t squarea , square2_t squareb );

    typedef struct
    {
        int col;
        int row;
        
        float g;
        float f;
        float h;
        
        char closed;
        char opened;
        char wall;

        void* parent;
    
    } path2_node_t;

    void path2_find( mtvec_t* nodes , mtvec_t* result , int cols , int rows , int start , int end );

    #endif
