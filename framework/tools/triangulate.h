

    #ifndef triangulate_h
    #define triangulate_h


    #include <stdio.h>
    #include "../core/mtvec.h"

    char    triangulate_process( mtvec_t* contour, mtvec_t* result );

    float   triangulate_area( mtvec_t* contour);

    char    triangulate_inside_triangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy,float Px, float Py);
    
    char    triangulate_snip( mtvec_t* contour, int u, int v, int w, int n, int *V );


    #endif
