

    #ifndef image_h
    #define image_h

    #include "../core/mtmem.h"
    #include "../core/mtbmp.h"
    #include "../ext/stb_image.h"

    mtbmp_t* image_bmp_from_png( char* path );

    #endif
