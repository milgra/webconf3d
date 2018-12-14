

    #ifndef C36F_GLmtbmp_h
    #define C36F_GLmtbmp_h


    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>

    typedef struct _mtbmp_t mtbmp_t;
    struct _mtbmp_t
    {
        int width;
        int height;

        uint8_t* bytes;
        uint32_t bytes_length;
    };

    mtbmp_t*    mtbmp_alloc( int the_width , int the_height );

    void        mtbmp_dealloc( void* the_bitmap );

    mtbmp_t*    mtbmp_alloc_from_bitmap( mtbmp_t* the_bitmap );

    mtbmp_t*    mtbmp_alloc_from_grayscale( int the_width, int the_height, uint32_t backcolor, uint32_t fontcolor, unsigned char* the_bitmap );

	mtbmp_t*    mtbmp_fill_with_color( mtbmp_t* bitmap, int the_sx, int the_sy, int the_ex, int the_ey, uint32_t color );

	void        mtbmp_replace_from( mtbmp_t* the_base , mtbmp_t* the_bitmap , int the_x , int the_y );

	void        mtbmp_blend_from( mtbmp_t* the_base , mtbmp_t* the_bitmap , int the_x , int the_y );


    #endif
