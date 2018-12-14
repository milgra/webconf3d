

    #include "math2.h"
    #include <assert.h>
	#include <float.h>
	#include "../core/mtmem.h"

	/* creates vector2 */

	vector2_t vector2_init( float x , float y )
	{
		vector2_t result;
		
		result.x = x;
		result.y = y;
		
		return result;
	}
	
	/* adds two vectors */

	vector2_t vector2_add( vector2_t a , vector2_t b )
	{
		vector2_t result;
		
		result.x = a.x + b.x;
		result.y = a.y + b.y;
		
		return result;
	}

	/* substracts b from a */
	
	vector2_t vector2_sub( vector2_t a , vector2_t b )
	{
		vector2_t result;
		
		result.x = a.x - b.x;
		result.y = a.y - b.y;
		
		return result;
	}

	/* multiplies vector with ratio */

	vector2_t vector2_scale( vector2_t vector , float ratio )
	{
		vector2_t result;

		result.x = vector.x * ratio;
		result.y = vector.y * ratio;
		
		return result;
	}

    /* resizes vector */

	vector2_t vector2_resize( vector2_t a , float size )
	{
        float length = sqrtf( a.x * a.x + a.y * a.y );
		float ratio = size / length;
		a.x *= ratio;
		a.y *= ratio;
		return a;
	}

    /* rotates vector with given radians */

	vector2_t vector2_rotate( vector2_t vector , float newangle )
	{
		float angle = vector2_angle_x( vector );
		float length = vector2_length( vector );
		angle += newangle;
		return vector2_init( cos( angle ) * length , sin( angle ) * length );
	}

    /* rotates vector to left with 90 degrees */

	vector2_t vector2_rotate_90_left( vector2_t vector )
	{
		return vector2_init( -vector.y , vector.x );
	}

    /* rotates vector to right with 90 degrees */

	vector2_t vector2_rotate_90_right( vector2_t vector )
	{
		return vector2_init( vector.y , -vector.x );
	}

    /* mirrors vector on axis */

    vector2_t vector2_mirror( vector2_t axis , vector2_t vector )
    {
        vector2_t projected_point = vector2_intersect_lines( vector2_init( 0.0, 0.0 ) , axis , vector , vector2_rotate_90_right( axis ) );

        vector2_t projected_vector = vector2_sub( projected_point , vector );
        return vector2_add( projected_point , projected_vector );
    }

    /* returns middle point */
    
    vector2_t vector2_midpoints( vector2_t pointa , vector2_t pointb )
    {
        vector2_t pointc = vector2_sub( pointb , pointa );
        
        pointc = vector2_scale( pointc , 0.5 );
        pointc = vector2_add( pointa , pointc );
        
        return pointc;
    }

	/* returns vector length */

	float vector2_length( vector2_t a )
    {
        return sqrtf( a.x * a.x + a.y * a.y );
    }

	/* returns vector angel on x axis */

	float vector2_angle_x( vector2_t a )
	{
		return atan2( a.y , a.x );
	}

    /* returns angle between two vector as a circular CCW angle where angle 0 is vector a */
    
    float vector2_circular_angle_between( vector2_t a , vector2_t b )
    {
        float anglea = vector2_angle_x(a);
        float angleb = vector2_angle_x(b);
        
        float anglere = angleb - anglea;
        if ( anglere < 0.0 ) anglere += 2 * M_PI;
        return anglere;
    }

    /* compares two vectors */

	char vector2_equals( vector2_t a , vector2_t b )
	{
		return a.x == b.x && a.y == b.y;
	}

	/* describes vector */

	void vector2_describe( vector2_t vector )
	{
		printf( "%f %f " , vector.x , vector.y );
	}

    /* returns longer side projected to x */

	float vector2_longside( vector2_t a )
    {
        float x = fabs( a.x );
        float y = fabs( a.y );
        return x > y ? x : y;
    }

    /* intersects lines by linear equation
       Ax + By = C
       A = y2 - y1
       B = x1 - x2
       C = A * x1 + B * y1 */

	vector2_t vector2_intersect_lines( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb )
	{
        if ( !(fabs( basisa.x ) > 0.0 || fabs( basisa.y ) > 0.0) ) return vector2_init( FLT_MAX , FLT_MAX );
        if ( !(fabs( basisb.x ) > 0.0 || fabs( basisb.y ) > 0.0) ) return vector2_init( FLT_MAX , FLT_MAX );
    
		float aA =  basisa.y;
		float aB = -basisa.x;

		float bA =  basisb.y;
		float bB = -basisb.x;

		float determinant = bA * aB - bB * aA;

		if ( determinant != 0 )
		{
			float aC = aA * transa.x + aB * transa.y;
			float bC = bA * transb.x + bB * transb.y;

			float x = ( aB * bC - bB * aC ) / determinant;
			float y = ( bA * aC - aA * bC ) / determinant;
			
			return vector2_init( x , y );
		}
        
        /* lines are parallel */
		
		return vector2_init( FLT_MAX , FLT_MAX );
	}    

	/* checking if point is inside a vector, the point must be on the line defined by the vector */

	char vector2_point_inside_vector( vector2_t transa , vector2_t basisa , vector2_t point )
	{
		float dx = point.x - transa.x;
		float dy = point.y - transa.y;
        
        // check sign similarity
		
		if ( basisa.x < 0 && dx >= 0 ) return 0;
		if ( basisa.x > 0 && dx < 0 ) return 0;
		if ( basisa.y < 0 && dy >= 0 ) return 0;
		if ( basisa.y > 0 && dy < 0 ) return 0;
        
        float absdx = fabs(dx);
        float absdy = fabs(dy);
        float absbx = fabs(basisa.x);
        float absby = fabs(basisa.y);
        
        // check length
        
        if ( absbx == 0.0 )
        {
            if ( absdx > 0.001 ) return 0;
        }
		else if ( absbx < absdx + 0.001 ) return 0;

        if ( absby == 0.0 )
        {
            if ( absdy > 0.001 ) return 0;
        }
		else if ( absby < absdy + 0.001 ) return 0;
		
		return 1;
	}

    /* intersect vectors */
    
	vector2_t vector2_intersect_vectors( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb )
	{
        vector2_t isp_inertia_segment = vector2_intersect_lines( transa , basisa , transb , basisb );

        /* lines are not parallel */
        
        if ( isp_inertia_segment.x != FLT_MAX &&
             isp_inertia_segment.y != FLT_MAX )
        {
            if ( vector2_point_inside_vector( transa , basisa , isp_inertia_segment ) == 1 &&
                 vector2_point_inside_vector( transb , basisb , isp_inertia_segment ) == 1 )
            {
                return isp_inertia_segment;
            }
        }
		return vector2_init( FLT_MAX , FLT_MAX );
	}



	// checking collosion of vector bounding boxes

	char vector2_box_intersect( vector2_t basisa , vector2_t transa , vector2_t basisb , vector2_t transb , float extra_distance )
	{
		// bounding box checking
		float dcx = fabs( ( transa.x + basisa.x / 2.0 ) - ( transb.x + basisb.x / 2.0 ) );
		float dcy = fabs( ( transa.y + basisa.y / 2.0 ) - ( transb.y + basisb.y / 2.0 ) );
		
		float maxx = fabs( basisa.x / 2.0 ) + fabs( basisb.x / 2.0 ) + extra_distance;
		float maxy = fabs( basisa.y / 2.0 ) + fabs( basisb.y / 2.0 ) + extra_distance;
	
		if ( dcx < maxx && dcy < maxy ) return 1;
		return 0;
	}

    /* !!! TODO CLEANUP, MERGE */

    float vector2_endpoint_proximity( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb )
    {
        // line intersection point
        vector2_t isp_inertia_segment = vector2_intersect_lines(
            transa ,
            basisa ,
            transb ,
            basisb );

        // lines are not parallel
        if ( isp_inertia_segment.x != FLT_MAX &&
             isp_inertia_segment.y != FLT_MAX )
        {
            vector2_t halfa = vector2_add( transa, vector2_scale( basisa , 0.5 ) );
            vector2_t halfb = vector2_add( transb, vector2_scale( basisb , 0.5 ) );

            vector2_t vectora = vector2_sub( halfa, isp_inertia_segment );
            vector2_t vectorb = vector2_sub( halfb, isp_inertia_segment );
            
            float asize = vector2_length( basisa ) / 2.0;
            float bsize = vector2_length( basisb ) / 2.0;
            float alength = vector2_length( vectora );
            float blength = vector2_length( vectorb );
            
            alength -= asize;
            blength -= bsize;
            
            if ( alength < 0.0 ) alength = 0.0;
            if ( blength < 0.0 ) blength = 0.0;
            
            return alength > blength ? alength : blength;
        }
        else
        {
            float dx = fabs(transa.x - transb.x);
            float dy = fabs(transa.y - transb.y);
            return dx > dy ? dx : dy;
        }
    }
	
	vector2_t vector2_intersect_with_proximity( vector2_t trans_a , vector2_t basis_a , vector2_t trans_b , vector2_t basis_b , float proximity )
	{
		vector2_t isp = vector2_intersect_vectors(
			trans_a ,
			basis_a ,
			trans_b ,
			basis_b );
        
		if ( isp.x == FLT_MAX && proximity > 0.0 )
		{
			float distance = vector2_endpoint_proximity( trans_a , basis_a , trans_b , basis_b );
			if ( distance < proximity ) isp = vector2_intersect_lines(
                trans_a ,
                basis_a ,
                trans_b ,
                basis_b );

		}
		
		return isp;
	}	

    float vector2_endpoint_nearby( vector2_t transa , vector2_t basisa , vector2_t transb  , vector2_t basisb )
    {
        // line intersection point
        vector2_t isp_inertia_segment = vector2_intersect_lines(
            transa ,
            basisa ,
            transb ,
            basisb );

        // lines are not parallel
        if ( isp_inertia_segment.x != FLT_MAX &&
             isp_inertia_segment.y != FLT_MAX )
        {
            /* endpoint part */
            
            vector2_t point_start = vector2_add( transa, basisa );
            vector2_t point_end = vector2_add( transb, vector2_scale( basisb , 0.5 ) );
            vector2_t connector = vector2_sub( point_end, point_start );
            
            float angle = vector2_circular_angle_between( connector , basisb );
            
            float dx = fabs(vector2_length( connector ) * cosf( angle ));
            float dy = fabs(vector2_length( connector ) * sinf( angle ));
            
            float basislength = vector2_length( basisb );
            
            if ( dx > basislength / 2.0 ) dx -= basislength / 2.0;
            else dx = 0.0;

            return dx > dy ? dx : dy;
        }
        else
        {
            float dx = fabs(transa.x - transb.x);
            float dy = fabs(transa.y - transb.y);
            return dx > dy ? dx : dy;
        }
    }

	vector2_t vector2_intersect_with_nearby( vector2_t trans_a , vector2_t basis_a , vector2_t trans_b , vector2_t basis_b , float proximity )
	{
		vector2_t isp = vector2_intersect_vectors(
			trans_a ,
			basis_a ,
			trans_b ,
			basis_b );
        
		if ( isp.x == FLT_MAX && proximity > 0.0 )
		{
			float distance = vector2_endpoint_nearby( trans_a , basis_a , trans_b , basis_b );
			if ( distance < proximity ) isp = vector2_intersect_lines(
                trans_a ,
                basis_a ,
                trans_b ,
                basis_b );

		}
		
		return isp;
	}	

    vector2_t vector2_triangle_with_bases( vector2_t point_a , vector2_t point_b , float segmentlength , int8_t direction )
    {
        vector2_t vector = vector2_sub( point_b , point_a );
        vector = vector2_scale( vector, .5 );
        float length = vector2_length( vector );
        if ( length < segmentlength )
        {
            float needed = sqrtf( segmentlength * segmentlength - length * length );
            vector2_t distance = vector2_init( (float)direction * -vector.y , (float)direction * vector.x );
            distance = vector2_resize( distance , needed );
            vector = vector2_add( point_a, vector);
            vector = vector2_add( vector , distance );
        }
        else vector = vector2_add( point_a, vector );
        return vector;
    }

    segment2_t vector2_collide_and_fragment( vector2_t transa , vector2_t basisa , vector2_t transb , vector2_t basisb )
    {
        vector2_t new_basis = vector2_mirror( basisb , basisa );
        vector2_t new_trans = vector2_intersect_lines( transa , basisa , transb , basisb );

        float basis_length = vector2_length( basisa );
        float chunk_length = vector2_length( vector2_sub( new_trans , transa ) );
        float final_length = basis_length - chunk_length;

        if ( final_length > 0.0 ) new_basis = vector2_resize( new_basis , final_length  );
        else new_basis = vector2_init( 0.0 , 0.0 );

        return segment2_init( new_trans , new_basis );
    }
	


    /* creates segment from two vectors */
    
    segment2_t segment2_init( vector2_t trans , vector2_t basis )
    {
        segment2_t result;
        result.trans = trans;
        result.basis = basis;
        return result;
    }

	/* creates square from two vectors */

	square2_t square_alloc( vector2_t origo , vector2_t extent )
	{
		square2_t square;
		square.origo = origo;
		square.extent = extent;
		return square;
	}

	/* returns intersection product */

	square2_t square_intersect( square2_t squarea , square2_t squareb )
	{
		float new_width		= squareb.extent.x;
		float new_height	= squareb.extent.y;
		float new_left		= fmin( squarea.origo.x , squareb.origo.x );
		float new_top		= fmax( squarea.origo.y , squareb.origo.y );

		if ( squareb.origo.x < squarea.origo.x )
		{
			new_width = squareb.origo.x + squareb.extent.x - squarea.origo.x;
			new_left = squarea.origo.x - squareb.origo.x;
		}

		if ( squareb.origo.y > squarea.origo.y )
		{
			new_height = squareb.origo.y + squareb.extent.y - squarea.origo.y;
			new_top = squarea.origo.y - squareb.origo.y;
		}

		if ( squareb.origo.x + squareb.extent.x > squarea.origo.x + squarea.extent.x )
		{
			new_width = squarea.origo.x + squarea.extent.x - squareb.origo.x;
		}

		if ( squareb.origo.y + squareb.extent.y < squarea.origo.y + squarea.extent.y )
		{
			new_height = squarea.origo.y + squarea.extent.y - squareb.origo.y;
		}

		if ( new_width  > squarea.extent.x ) new_width  = squarea.extent.x;
		if ( new_height < squarea.extent.y ) new_height = squarea.extent.y;

		square2_t result;
		result.origo = vector2_init( new_left , new_top );
		result.extent = vector2_init( new_width , new_height );
		
		return result;
	}

	/*  checks overlapping
        returns 0 if theres no overlapping
        returns 1 if squareb is inside squarea
        returns 2 if there is partial overlapping */

	uint8_t	square_checkoverlapping( square2_t squarea , square2_t squareb )
	{
		if ( squareb.origo.x + squareb.extent.x < squarea.origo.x ||
			 squareb.origo.x > squarea.origo.x + squarea.extent.x ||
			 squareb.origo.y + squareb.extent.y > squarea.origo.y ||
			 squareb.origo.y < squarea.origo.y + squarea.extent.y )
		{
			return kSquareOverlappingNone;
		}
		else
		if ( squareb.origo.x >= squarea.origo.x &&
			 squareb.origo.x + squareb.extent.x <= squarea.origo.x + squarea.extent.x &&
			 squareb.origo.y <= squarea.origo.y &&
			 squareb.origo.y + squareb.extent.y >= squarea.origo.y + squarea.extent.y )
		{
			return kSquareOverlappingInside;
		}
		else return kSquareOverlappingPartial;
	}


    // adds node to openlist keeping f order

    void path2_addnode( mtvec_t* openList , path2_node_t* node )
    {
        char added = 0;
        for ( int i = 0 ; i < openList->length ; i++ )
        {
            path2_node_t* actual = openList->data[ i ];
            if ( node->f > actual->f )
            {
                mtvec_adddataatindex( openList , node , i );
                added = 1;
                break;
            }
        }
        if ( added == 0 )
        {
            mtvec_adddata( openList , node );
        }
    }

    /* finds path between start and end */

    void path2_find( mtvec_t* nodes , mtvec_t* result , int cols , int rows , int start , int end )
    {
        path2_node_t* startNode = nodes->data[ start ];
        path2_node_t* endNode = nodes->data[ end ];
        
        mtvec_t* openList = result;
        
        float weight = 1.0; // speed up heuristics

        startNode->g = 0.0;
        startNode->f = 0.0;
        
        mtvec_adddata( openList , startNode );

        startNode->opened = 1;
        
        while ( openList->length > 0 )
        {
            // pop the position of node which has the minimum `f` value.
            path2_node_t* node = mtvec_tail( openList );
            mtvec_removedataatindex( openList , openList->length - 1 );
            node->closed = 1;

            // if reached the end position, construct the path and return it
            if (node == endNode)
            {
                int border = openList->length;
                path2_node_t* tempNode = endNode;
                mtvec_adddata( openList , tempNode );
                while ( tempNode->parent != NULL )
                {
                    tempNode = tempNode->parent;
                    mtvec_adddata( openList , tempNode );
                }
                // clear temporary values
                mtvec_removedatainrange( openList , 0 , border - 1 );
                mtvec_reverse( openList );
                return;
            }

            // get neigbours of the current node
            mtvec_t* neighbours = mtvec_alloc( );
            
            int northindex = ( ( node->row - 1 ) * cols ) + node->col;
            int southindex = ( ( node->row + 1 ) * cols ) + node->col;
            int eastindex = ( node->row * cols ) + node->col - 1;
            int westindex = ( node->row * cols ) + node->col + 1;
            int neindex = ( ( node->row - 1 ) * cols ) + node->col - 1;
            int nwindex = ( ( node->row - 1 ) * cols ) + node->col + 1;
            int seindex = ( ( node->row + 1 ) * cols ) + node->col - 1;
            int swindex = ( ( node->row + 1 ) * cols ) + node->col + 1;

            if ( node->col > 0 ) mtvec_adddata( neighbours , nodes->data[ eastindex ] );
            if ( node->col + 1 < cols ) mtvec_adddata( neighbours , nodes->data[ westindex ] );
            if ( node->row > 0 ) mtvec_adddata( neighbours , nodes->data[ northindex ] );
            if ( node->row + 1 < rows ) mtvec_adddata( neighbours , nodes->data[ southindex ] );
            if ( neindex > 0 && neindex < nodes->length ) mtvec_adddata( neighbours , nodes->data[ neindex ] );
            if ( nwindex > 0 && nwindex < nodes->length ) mtvec_adddata( neighbours , nodes->data[ nwindex ] );
            if ( seindex > 0 && seindex < nodes->length ) mtvec_adddata( neighbours , nodes->data[ seindex ] );
            if ( swindex > 0 && swindex < nodes->length ) mtvec_adddata( neighbours , nodes->data[ swindex ] );

            for ( int index = 0 ; index < neighbours->length ; ++index )
            {
                path2_node_t* neighbor = neighbours->data[ index ];
                
                if ( neighbor->closed == 1 ) {
                    continue;
                }

                // get the distance between current node and the neighbor
                // and calculate the next g score
                float ng = node->g + ( ( neighbor->col == node->col || neighbor->row == node->row ) ? 1 : sqrtf( 2.0 ) );

                // check if the neighbor has not been inspected yet, or
                // can be reached with smaller cost from the current node
                if ( neighbor->opened == 0 || ng < neighbor->g )
                {
                    int diffX = abs( neighbor->col - endNode->col );
                    int diffY = abs( neighbor->row - endNode->row );
                    int heuristic = diffX < diffY ? diffY : diffX;  // chebysev

                    if ( weight * heuristic > neighbor->h ) neighbor->h = weight * heuristic;
                    neighbor->g = ng;
                    neighbor->f = neighbor->g + neighbor->h;
                    neighbor->parent = node;

                    if ( neighbor->opened == 0 )
                    {
                        neighbor->opened = 1;
                        path2_addnode( openList , neighbor );
                    }
                    else
                    {
                        // the neighbor can be reached with smaller cost.
                        // Since its f value has been updated, we have to
                        // update its position in the open list
                        mtvec_removedata( openList, neighbor );
                        path2_addnode( openList , neighbor );
                    }
                }
            } // end for each neighbor
            
            mtmem_release( neighbours );
        } // end while not open list empty

        printf( "no path found :(\n" );
        // fail to find the path
        return;
    }

