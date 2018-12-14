

    #ifndef __c36__math3__
    #define __c36__math3__


	#include <stdio.h>
	#include <math.h>


	typedef struct _vector3_t vector3_t;
	struct _vector3_t
	{
		float x, y, z;
	};

	vector3_t	vector3_init( float x , float y , float z );

	vector3_t	vector3_add( vector3_t a , vector3_t b );

	vector3_t	vector3_sub( vector3_t a , vector3_t b );

	vector3_t	vector3_scale( vector3_t a , float f );

	vector3_t	vector3_cross( vector3_t left , vector3_t right);

	vector3_t	vector3_normalize( vector3_t matrix );

    vector3_t	vector3_rotatearoundx( vector3_t vector , float the_angle );

    vector3_t	vector3_rotatearoundy( vector3_t vector , float the_angle );

    vector3_t	vector3_rotatearoundz( vector3_t vector , float the_angle );

	vector3_t	vector3_getxyunitrotation( vector3_t vx , vector3_t vy );

	vector3_t 	vector3_intersectwithplane( vector3_t linev1 , vector3_t linev2 , vector3_t planev , vector3_t planen );

	float		vector3_dot( vector3_t a , vector3_t b );

	float		vector3_angle( vector3_t a , vector3_t b );

	float		vector3_length( vector3_t a );

    void		vector3_toarray( vector3_t* vector , float* result );

    float		vector3_distance( vector3_t vectorA , vector3_t vectorB );


	typedef struct _matrix3_t matrix3_t;
	struct _matrix3_t
	{
		float m00, m01, m02;
		float m10, m11, m12;
		float m20, m21, m22;
	};

	matrix3_t	matrix3_defaultidentity(void);

    matrix3_t	matrix3_defaultscale( float x , float y );

	matrix3_t	matrix3_defaulttranslation( float x , float y );

	matrix3_t	matrix3_defaultrotationx( float radian );

	matrix3_t	matrix3_defaultrotationy( float radian );

	matrix3_t	matrix3_defaultrotationz( float radian );

	matrix3_t	matrix3_multiply( matrix3_t a , matrix3_t b );

	matrix3_t	matrix3_invert( matrix3_t source , char* success );

	matrix3_t	matrix3_transpose( matrix3_t matrix );

	vector3_t	matrix3_multiply_vector3( matrix3_t matrix , vector3_t vector );

//	vector3_t	matrix3_multiply_vector3_transposed( matrix3_t matrix , vector3_t vector );

	void		matrix3_multiplywithnumber( matrix3_t* matrix , float number );

	void		matrix3_describe( matrix3_t matrix );


	typedef struct _transformation_t transformation_t;
	struct _transformation_t
	{
		vector3_t translation;
		vector3_t rotation;
		vector3_t scale;
	};

	transformation_t transformation_alloc(void);

	
    #endif
