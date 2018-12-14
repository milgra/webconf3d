

	#include "math4.h"
    #include <math.h>
	#include <float.h>


	void matrix4_multiplywithnumber( matrix4_t* matrix , float number );


	/* creates vector */

	vector4_t vector4_init( float x , float y , float z , float w )
	{
		vector4_t result;
		
		result.x = x;
		result.y = y;
		result.z = z;
		result.w = w;
		
		return result;
	}

    /* adds two vectors */

	vector4_t vector4_add( vector4_t a , vector4_t b )
	{
		vector4_t result;
		
		result.x = a.x + b.x;
		result.y = a.y + b.y;
		result.z = a.z + b.z;
		result.w = a.w + b.w;
		
		return result;
	}

	/* substract b from a */

    vector4_t vector4_sub( vector4_t a , vector4_t b )
    {
		vector4_t result;
		
		result.x = a.x - b.x;
		result.y = a.y - b.y;
		result.z = a.z - b.z;
		result.w = a.w - b.w;
		
		return result;
    }

	/* scales vector */

	vector4_t vector4_scale( vector4_t a , float f )
	{
		vector4_t result;

		result.x = a.x * f;
		result.y = a.y * f;
		result.z = a.z * f;
		result.w = a.w * f;
		
		return result;
	}

	/* describes vector4 */

	void vector4_describe( vector4_t vector )
	{
		printf( "x : %f y : %f z : %f w : %f" , vector.x , vector.y , vector.z , vector.w );
	}

	/* creates identity matrix */

	matrix4_t matrix4_defaultidentity( )
	{
		matrix4_t matrix;
		
		matrix.m00 = 1.0f;
		matrix.m01 = 0.0f;
		matrix.m02 = 0.0f;
		matrix.m03 = 0.0f;
		matrix.m10 = 0.0f;
		matrix.m11 = 1.0f;
		matrix.m12 = 0.0f;
		matrix.m13 = 0.0f;
		matrix.m20 = 0.0f;
		matrix.m21 = 0.0f;
		matrix.m22 = 1.0f;
		matrix.m23 = 0.0f;
		matrix.m30 = 0.0f;
		matrix.m31 = 0.0f;
		matrix.m32 = 0.0f;
		matrix.m33 = 1.0f;
		
		return matrix;
	}

    /* creates scale matrix */

    matrix4_t matrix4_defaultscale( float x , float y , float z )
    {
        matrix4_t matrix = matrix4_defaultidentity( );
    
		matrix.m00 = x;
		matrix.m11 = y;
		matrix.m22 = z;
		
		return matrix;
    }

	/* creates rotation matrix */

	matrix4_t matrix4_defaultrotation( float x , float y , float z )
	{
		float max = fabs(x) > fabs(y) ? x : y;
		max = fabs(z) > fabs(max) ? z : max;
		
		if ( max == 0.0 ) return matrix4_defaultidentity();

		x = x / max;
		y = y / max;
		z = z / max;

		float nx, ny, nz, scale, sin, cos, cosp;
		matrix4_t matrix;
		
		// normalize values
		
		nx = x;
		ny = y;
		nz = z;

		scale = 1.0f / sqrtf( nx * nx + ny * ny + nz * nz );

		nx *= scale;
		ny *= scale;
		nz *= scale;
		
		// precalc
		
		sin = sinf( max );
		cos = cosf( max );
		cosp = 1.0f - cos;
		
		// create matrix
		
		matrix.m00 = cos + cosp * nx * nx;
		matrix.m01 = cosp * nx * ny + nz * sin;
		matrix.m02 = cosp * nx * nz - ny * sin;
		matrix.m03 = 0.0f;
		matrix.m10 = cosp * nx * ny - nz * sin;
		matrix.m11 = cos + cosp * ny * ny;
		matrix.m12 = cosp * ny * nz + nx * sin;
		matrix.m13 = 0.0f;
		matrix.m20 = cosp * nx * nz + ny * sin;
		matrix.m21 = cosp * ny * nz - nx * sin;
		matrix.m22 = cos + cosp * nz * nz;
		matrix.m23 = 0.0f;
		matrix.m30 = 0.0f;
		matrix.m31 = 0.0f;
		matrix.m32 = 0.0f;
		matrix.m33 = 1.0f;
		
		return matrix;
	}

	/* creates translation matrix */
	
	matrix4_t matrix4_defaulttranslation( float x , float y , float z )
	{
		matrix4_t result;
		
		result = matrix4_defaultidentity( );
        result.m00 = 1;
		
		result.m30 = x;
		result.m31 = y;
		result.m32 = z;

		return result;
	}

	/* creates ortographic projection */

	matrix4_t matrix4_defaultortho( float left , float right , float bottom , float top , float near , float far )
	{
		float rpl, rml, tpb, tmb, fpn, fmn;
		matrix4_t matrix;

		rpl = right + left;
		rml = right - left;
		tpb = top + bottom;
		tmb = top - bottom;
		fpn = far + near;
		fmn = far - near;
						
		matrix.m00 = 2.0f / rml;
		matrix.m01 = 0.0f;
		matrix.m02 = 0.0f;
		matrix.m03 = 0.0f;
		matrix.m10 = 0.0f;
		matrix.m11 = 2.0f / tmb;
		matrix.m12 = 0.0f;
		matrix.m13 = 0.0f;
		matrix.m20 = 0.0f;
		matrix.m21 = 0.0f;
		matrix.m22 = -2.0f / fmn;
		matrix.m23 = 0.0f;
		matrix.m30 = -rpl / rml;
		matrix.m31 = -tpb / tmb;
		matrix.m32 = -fpn / fmn;
		matrix.m33 = 1.0f;
		
		return matrix;
	}

	/* create perspective projection */

	matrix4_t matrix4_defaultperspective( float fovy , float aspect , float nearz , float farz )
	{
		float cotan;
		matrix4_t matrix;
			
		cotan = 1.0f / tanf(fovy / 2.0f);
		
		matrix.m00 = cotan / aspect;
		matrix.m01 = 0.0f;
		matrix.m02 = 0.0f;
		matrix.m03 = 0.0f;
		
		matrix.m10 = 0.0f;
		matrix.m11 = cotan;
		matrix.m12 = 0.0f;
		matrix.m13 = 0.0f;
		
		matrix.m20 = 0.0f;
		matrix.m21 = 0.0f;
		matrix.m22 = (farz + nearz) / (nearz - farz);
		matrix.m23 = -1.0f;
		
		matrix.m30 = 0.0f;
		matrix.m31 = 0.0f;
		matrix.m32 = (2.0f * farz * nearz) / (nearz - farz);
		matrix.m33 = 0.0f;
		
		return matrix;
	}

	/* scales matrix */
	
	matrix4_t matrix4_scale( matrix4_t matrix ,	float x , float	y , float z )
	{
		matrix.m00 *= x;
		matrix.m11 *= y;
		matrix.m22 *= z;
		
		return matrix;
	}

	/* rotates matrix */

	matrix4_t matrix4_rotate( matrix4_t	matrix , float x , float y , float z )
	{
		matrix4_t rotation;
		
		rotation = matrix4_defaultrotation( x , y , z );
		return matrix4_multiply( matrix , rotation );
	}

	/* translates matrix */
	
	matrix4_t matrix4_translate( matrix4_t other , float x , float y , float z )
	{
		other.m30 = other.m00 * x + other.m10 * y + other.m20 * z + other.m30;
		other.m31 = other.m01 * x + other.m11 * y + other.m21 * z + other.m31;
		other.m32 = other.m02 * x + other.m12 * y + other.m22 * z + other.m32;
		
		return other;
	}

	/* inverts matrix */
	
	matrix4_t matrix4_invert( matrix4_t source , char* success )
	{
		float determinant;
		matrix4_t inverse;

		inverse.m00 = source.m11 * source.m22 * source.m33 -
					  source.m11 * source.m23 * source.m32 -
					  source.m21 * source.m12 * source.m33 +
					  source.m21 * source.m13 * source.m32 +
					  source.m31 * source.m12 * source.m23 -
					  source.m31 * source.m13 * source.m22;

		inverse.m10 = -source.m10* source.m22 * source.m33 +
					  source.m10 * source.m23 * source.m32 +
					  source.m20 * source.m12 * source.m33 -
					  source.m20 * source.m13 * source.m32 -
					  source.m30 * source.m12 * source.m23 +
					  source.m30 * source.m13 * source.m22;

		inverse.m20 = source.m10 * source.m21 * source.m33 -
					  source.m10 * source.m23 * source.m31 -
					  source.m20 * source.m11 * source.m33 +
					  source.m20 * source.m13 * source.m31 +
					  source.m30 * source.m11 * source.m23 -
					  source.m30 * source.m13 * source.m21;

		inverse.m30 = -source.m10* source.m21 * source.m32 +
					  source.m10 * source.m22 * source.m31 +
					  source.m20 * source.m11 * source.m32 -
					  source.m20 * source.m12 * source.m31 -
					  source.m30 * source.m11 * source.m22 +
					  source.m30 * source.m12 * source.m21;

		inverse.m01 = -source.m01* source.m22 * source.m33 +
					  source.m01 * source.m23 * source.m32 +
					  source.m21 * source.m02 * source.m33 -
					  source.m21 * source.m03 * source.m32 -
					  source.m31 * source.m02 * source.m23 + 
					  source.m31 * source.m03 * source.m22;

		inverse.m11 = source.m00 * source.m22 * source.m33 -
					  source.m00 * source.m23 * source.m32 -
					  source.m20 * source.m02 * source.m33 +
					  source.m20 * source.m03 * source.m32 +
					  source.m30 * source.m02 * source.m23 -
					  source.m30 * source.m03 * source.m22;

		inverse.m21 = -source.m00* source.m21 * source.m33 +
					  source.m00 * source.m23 * source.m31 +
					  source.m20 * source.m01 * source.m33 -
					  source.m20 * source.m03 * source.m31 -
					  source.m30 * source.m01 * source.m23 + 
					  source.m30 * source.m03 * source.m21;

		inverse.m31 = source.m00 * source.m21 * source.m32 -
					  source.m00 * source.m22 * source.m31 -
					  source.m20 * source.m01 * source.m32 +
					  source.m20 * source.m02 * source.m31 +
					  source.m30 * source.m01 * source.m22 - 
					  source.m30 * source.m02 * source.m21;

		inverse.m02 = source.m01 * source.m12 * source.m33 -
					  source.m01 * source.m13 * source.m32 -
					  source.m11 * source.m02 * source.m33 +
					  source.m11 * source.m03 * source.m32 +
					  source.m31 * source.m02 * source.m13 -
					  source.m31 * source.m03 * source.m12;

		inverse.m12 = -source.m00* source.m12 * source.m33 +
					  source.m00 * source.m13 * source.m32 +
					  source.m10 * source.m02 * source.m33 -
					  source.m10 * source.m03 * source.m32 -
					  source.m30 * source.m02 * source.m13 + 
					  source.m30 * source.m03 * source.m12;

		inverse.m22 = source.m00 * source.m11 * source.m33 -
					  source.m00 * source.m13 * source.m31 -
					  source.m10 * source.m01 * source.m33 +
					  source.m10 * source.m03 * source.m31 +
					  source.m30 * source.m01 * source.m13 - 
					  source.m30 * source.m03 * source.m11;

		inverse.m32 = -source.m00* source.m11 * source.m32 +
					  source.m00 * source.m12 * source.m31 +
					  source.m10 * source.m01 * source.m32 -
					  source.m10 * source.m02 * source.m31 -
					  source.m30 * source.m01 * source.m12 +
					  source.m30 * source.m02 * source.m11;

		inverse.m03 = -source.m01* source.m12 * source.m23 +
					  source.m01 * source.m13 * source.m22 +
					  source.m11 * source.m02 * source.m23 - 
					  source.m11 * source.m03 * source.m22 - 
					  source.m21 * source.m02 * source.m13 + 
					  source.m21 * source.m03 * source.m12;

		inverse.m13 = source.m00 * source.m12 * source.m23 -
					  source.m00 * source.m13 * source.m22 -
					  source.m10 * source.m02 * source.m23 +
					  source.m10 * source.m03 * source.m22 +
					  source.m20 * source.m02 * source.m13 -
					  source.m20 * source.m03 * source.m12;

		inverse.m23 = -source.m00* source.m11 * source.m23 +
					  source.m00 * source.m13 * source.m21 +
					  source.m10 * source.m01 * source.m23 -
					  source.m10 * source.m03 * source.m21 -
					  source.m20 * source.m01 * source.m13 +
					  source.m20 * source.m03 * source.m11;

		inverse.m33 = source.m00 * source.m11 * source.m22 -
					  source.m00 * source.m12 * source.m21 -
					  source.m10 * source.m01 * source.m22 + 
					  source.m10 * source.m02 * source.m21 + 
					  source.m20 * source.m01 * source.m12 - 
					  source.m20 * source.m02 * source.m11;

		determinant = source.m00 * inverse.m00 +
					  source.m01 * inverse.m10 +
					  source.m02 * inverse.m20 +
					  source.m03 * inverse.m30;
		
		if ( determinant == 0 && success != NULL ) *success = 0;

		matrix4_multiplywithnumber( &inverse , 1.0 / determinant );
			
		return inverse;
	}

	/* multiplies matrices */

	matrix4_t matrix4_multiply( matrix4_t a , matrix4_t b )
	{
		matrix4_t matrix;
		
		matrix.m00 = a.m00 * b.m00 + a.m10 * b.m01 + a.m20 * b.m02 + a.m30 * b.m03;
		matrix.m10 = a.m00 * b.m10 + a.m10 * b.m11 + a.m20 * b.m12 + a.m30 * b.m13;
		matrix.m20 = a.m00 * b.m20 + a.m10 * b.m21 + a.m20 * b.m22 + a.m30 * b.m23;
		matrix.m30 = a.m00 * b.m30 + a.m10 * b.m31 + a.m20 * b.m32 + a.m30 * b.m33;
		
		matrix.m01 = a.m01 * b.m00 + a.m11 * b.m01 + a.m21 * b.m02 + a.m31 * b.m03;
		matrix.m11 = a.m01 * b.m10 + a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13;
		matrix.m21 = a.m01 * b.m20 + a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23;
		matrix.m31 = a.m01 * b.m30 + a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33;
		
		matrix.m02 = a.m02 * b.m00 + a.m12 * b.m01 + a.m22 * b.m02 + a.m32 * b.m03;
		matrix.m12 = a.m02 * b.m10 + a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13;
		matrix.m22 = a.m02 * b.m20 + a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23;
		matrix.m32 = a.m02 * b.m30 + a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33;
		
		matrix.m03 = a.m03 * b.m00 + a.m13 * b.m01 + a.m23 * b.m02 + a.m33 * b.m03;
		matrix.m13 = a.m03 * b.m10 + a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13;
		matrix.m23 = a.m03 * b.m20 + a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23;
		matrix.m33 = a.m03 * b.m30 + a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33;
		
		return matrix;
	}

	/* transposes matrix */
	
	matrix4_t matrix4_transpose( matrix4_t matrix )
	{
		matrix4_t result;

		result.m00 = matrix.m00;
		result.m11 = matrix.m11;
		result.m22 = matrix.m22;
		result.m33 = matrix.m33;
		
		result.m10 = matrix.m01;
		result.m01 = matrix.m10;
		result.m20 = matrix.m02;
		result.m02 = matrix.m20;
		result.m30 = matrix.m03;
		result.m03 = matrix.m30;
		
		result.m21 = matrix.m12;
		result.m12 = matrix.m21;
		result.m31 = matrix.m13;
		result.m13 = matrix.m31;
		result.m32 = matrix.m23;
		result.m23 = matrix.m32;
		
		return result;
	}

	/* describes matrix */

	void matrix4_describe( matrix4_t matrix )
	{
		printf( "%.2f %.2f %.2f %.2f | %.2f %.2f %.2f %.2f | %.2f %.2f %.2f %.2f | %.2f %.2f %.2f %.2f\n" ,
				matrix.m00 ,
				matrix.m01 ,
				matrix.m02 ,
				matrix.m03 ,
				matrix.m10 ,
				matrix.m11 ,
				matrix.m12 ,
				matrix.m13 ,
				matrix.m20 ,
				matrix.m21 ,
				matrix.m22 ,
				matrix.m23 ,
				matrix.m30 ,
				matrix.m31 ,
				matrix.m32 ,
				matrix.m33 );
	}

	/* extract rotation angles from matrix */

    void matrix4_extractangles( matrix4_t matrix , float* x , float* y , float* z )
    {
        float y1,y2,x1,x2,z1,z2;
        x1 = x2 = y1 = y2 = z1 = z2 = 0.0;

        if ( fabs(matrix.m20) !=  1 )
        {
            y1 = -asinf( matrix.m20 );
            y2 = M_PI - y1;
            x1 = atan2( matrix.m21 / cosf( y1 ) , matrix.m22 / cos( y1 ) );
            x2 = atan2( matrix.m21 / cosf( y2 ) , matrix.m22 / cos( y2 ) );
            z1 = atan2( matrix.m10 / cosf( y1 ) , matrix.m00 / cos( y1 ) );
            z2 = atan2( matrix.m10 / cosf( y2 ) , matrix.m00 / cos( y2 ) );
        }
        else
        {
            z1 = 0;
            if ( matrix.m20 == -1.0 )
            {
                y1 = M_PI / 2.0;
                x1 = z1 + atan2( matrix.m01 , matrix.m02 );
            }
            else
            {
                y1 = -M_PI / 2.0;
                x1 = -z1 + atan2( -matrix.m01 , -matrix.m02 );
            }
        }
        // printf( "angles %f %f %f , %f %f %f" , x1 * 180 / M_PI , y1 * 180 / M_PI, z1* 180 / M_PI , x2* 180 / M_PI , y2* 180 / M_PI , z2* 180 / M_PI );
        *x = x1;
        *y = y1;
        *z = z1;
    }

	/* internal : multiplies with number */

	void matrix4_multiplywithnumber( matrix4_t* matrix , float number )
	{
		matrix->m00 *= number;
		matrix->m01 *= number;
		matrix->m02 *= number;
		matrix->m03 *= number;
		matrix->m10 *= number;
		matrix->m11 *= number;
		matrix->m12 *= number;
		matrix->m13 *= number;
		matrix->m20 *= number;
		matrix->m21 *= number;
		matrix->m22 *= number;
		matrix->m23 *= number;
		matrix->m30 *= number;
		matrix->m31 *= number;
		matrix->m32 *= number;
		matrix->m33 *= number;
	}

    

	/* multiplies matrix4 with vector 4 */

	vector4_t matrix4_multiply_vector4( matrix4_t matrix , vector4_t vector )
	{
		vector4_t result;

    	result.x = matrix.m00 * vector.x + matrix.m10  * vector.y + matrix.m20 * vector.z + matrix.m30 * vector.w;
		result.y = matrix.m01 * vector.x + matrix.m11  * vector.y + matrix.m21 * vector.z + matrix.m31 * vector.w;
		result.z = matrix.m02 * vector.x + matrix.m12  * vector.y + matrix.m22 * vector.z + matrix.m32 * vector.w;
		result.w = matrix.m03 * vector.x + matrix.m13  * vector.y + matrix.m23 * vector.z + matrix.m33 * vector.w;

	    return result;
	}
    
	/* projects model space vector4 to screen space */

	vector4_t matrix4_world_to_screen_coords( matrix4_t matrix , vector4_t srcvector , float width , float height )
	{
		vector4_t vector;
		vector.x = srcvector.x;
		vector.y = srcvector.y;
		vector.z = 0;
		vector.w = 1;
		
		vector = matrix4_multiply_vector4( matrix , vector );

		if ( vector.w == 0 ) return vector;

		// perspective divide to normalized device coordinates
		
		vector.x /= vector.w;
		vector.y /= vector.w;
		vector.z /= vector.w;
		
		// viewport transformation

		vector4_t result;
		result.x = ( vector.x + 1.0 ) * width * 0.5;
		result.y = ( vector.y + 1.0 ) * height * 0.5;
		result.z = vector.z;
		result.w = vector.w;
		
		return result;
	}

	/* projects screen space vector4 to model space */

	vector3_t matrix4_screen_to_world_coords( matrix4_t mvpmatrix , vector4_t scrvector , float width , float height )
	{
		// get normalized device coordinates
        // ( src.x - ( width / 2.0 ) ) / ( width / 2.0 )
        // ( src.y - ( height / 2.0 ) ) / ( height / 2.0 )

		vector4_t vector;
		vector.x = scrvector.x / width * 2.0 - 1.0;
		vector.y = scrvector.y / height * 2.0 - 1.0;
		vector.z = scrvector.z;
		vector.w = 1.0;
		
		// invert projection matrix

		char success = 1;
		matrix4_t invert = matrix4_invert( mvpmatrix , &success );
		
		// multiply transposed inverted projection matrix with vector

		vector4_t result = matrix4_multiply_vector4( invert , vector );

		if ( result.w == 0 ) return vector3_init( FLT_MAX, FLT_MAX, FLT_MAX );
		
		// get world space coordinates

		result.w = 1.0 / result.w;
		result.x *= result.w;
		result.y *= result.w;
		result.z *= result.w;

		return vector3_init( result.x , result.y , result.z );
    }

	/* extracts data from matrix4 */

	void matrix4_extract( matrix4_t trafo , vector3_t* scale , vector3_t* rotation , vector3_t* translation )
    {
        vector4_t base_o = vector4_init( 0 , 0 , 0 , 1 );
        vector4_t base_x = vector4_init( 1 , 0 , 0 , 1 );
        vector4_t base_y = vector4_init( 0 , 1 , 0 , 1 );

        base_o = matrix4_multiply_vector4( trafo , base_o );
        base_x = matrix4_multiply_vector4( trafo , base_x );
        base_y = matrix4_multiply_vector4( trafo , base_y );

        vector3_t final_o = vector3_init( base_o.x , base_o.y , base_o.z );
        vector3_t final_x = vector3_init( base_x.x , base_x.y , base_x.z );
        vector3_t final_y = vector3_init( base_y.x , base_y.y , base_y.z );
        
        translation->x = final_o.x;
        translation->y = final_o.y;
        translation->z = final_o.z;
        
        final_x = vector3_sub( final_x , final_o );
        final_y = vector3_sub( final_y , final_o );
        
        float scale_factor = vector3_length( final_x );
        
        scale->x = scale_factor;
        scale->y = scale_factor;
        scale->z = scale_factor;
        
        *rotation = vector3_getxyunitrotation( final_x , final_y );
    }

    /* returns point_is coordiantes in the local coordinate system of the quad described by ulc, urc, llc */
	
	vector3_t vector4_quadrelativecoors( vector4_t ulc , vector4_t urc , vector4_t llc , vector3_t point_is )
	{
        vector3_t plane_a, plane_b, plane_d;
        vector3_t mtvec_ab, mtvec_ad, mtvec_n, mtvec_ai;

        plane_a = vector3_init( ulc.x , ulc.y , ulc.z );
        plane_b = vector3_init( llc.x , llc.y , llc.z );
        plane_d = vector3_init( urc.x , urc.y , urc.z );

        // create plane vectors and normal

        mtvec_ab = vector3_sub( plane_b , plane_a );
        mtvec_ad = vector3_sub( plane_d , plane_a );
        mtvec_n  = vector3_cross( mtvec_ab , mtvec_ad );

        // get angle of AI from AB and AC to build up the frame square in its actual position

        mtvec_ai = vector3_sub( point_is , plane_a );

        float angle_ab_ai = vector3_angle( mtvec_ab , mtvec_ai );
        float angle_ad_ai = vector3_angle( mtvec_ad , mtvec_ai );
        float length_mtvec_ai = vector3_length( mtvec_ai );

        // get relative coordinates

        vector3_t relative;
        relative.x = cos( angle_ad_ai ) * length_mtvec_ai;
        relative.y = -sin( angle_ad_ai ) * length_mtvec_ai;

        // check quarters

        if ( angle_ab_ai > M_PI / 2 && angle_ad_ai > M_PI / 2 ) relative.y *= -1;
        else if ( angle_ab_ai > M_PI / 2 && angle_ad_ai < M_PI / 2 ) relative.y *= -1;

		if ( relative.x > 0.0 && relative.x < vector3_length( mtvec_ad ) &&
			 relative.y < 0.0 && relative.y > -vector3_length( mtvec_ab ) )
		{
			return relative;
		}
		else return vector3_init( FLT_MAX , FLT_MAX , FLT_MAX );
	}

	/* calculates intersection point of quad and line */

	vector3_t vector4_quadlineintersection( vector4_t ulc , vector4_t urc , vector4_t llc , vector3_t linea , vector3_t lineb )
	{
        vector3_t plane_a, plane_b, plane_d;
        vector3_t mtvec_ab, mtvec_ad, mtvec_n, mtvec_ai, point_is;

        plane_a = vector3_init( ulc.x , ulc.y , ulc.z );
        plane_b = vector3_init( llc.x , llc.y , llc.z );
        plane_d = vector3_init( urc.x , urc.y , urc.z );

        // create plane vectors and normal

        mtvec_ab = vector3_sub( plane_b , plane_a );
        mtvec_ad = vector3_sub( plane_d , plane_a );
        mtvec_n  = vector3_cross( mtvec_ab , mtvec_ad );

        // get intersection point

        point_is = vector3_intersectwithplane( linea , lineb , plane_a , mtvec_n );

        // get angle of AI from AB and AC to build up the frame square in its actual position

        mtvec_ai = vector3_sub( point_is , plane_a );

        float angle_ab_ai = vector3_angle( mtvec_ab , mtvec_ai );
        float angle_ad_ai = vector3_angle( mtvec_ad , mtvec_ai );
        float length_mtvec_ai = vector3_length( mtvec_ai );

        // get relative coordinates

        float x = cos( angle_ad_ai ) * length_mtvec_ai;
        float y = -sin( angle_ad_ai ) * length_mtvec_ai;

        // check quarters

        if ( angle_ab_ai > M_PI / 2 && angle_ad_ai > M_PI / 2 ) y *= -1;
        else if ( angle_ab_ai > M_PI / 2 && angle_ad_ai < M_PI / 2 ) y *= -1;

		if ( x > 0.0 && x < vector3_length( mtvec_ad ) && y < 0.0 && y > -vector3_length( mtvec_ab ) ) return point_is;
		else return vector3_init( FLT_MAX , FLT_MAX , FLT_MAX );
	}

