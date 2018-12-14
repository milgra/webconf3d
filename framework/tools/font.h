

    #ifndef C36F_TLFont_h
    #define C36F_TLFont_h

    #include <stdint.h>
	#include "../core/mtbmp.h"
    #include "../core/mtstr.h"
    #include "../core/mtvec.h"
    #include "../ext/stb_truetype.h"
    #include "../ui/text.h"


    typedef struct _font_t font_t;
    struct _font_t
    {
        stbtt_fontinfo info;
        unsigned char* buffer;
        int gap;
        int ascent;
        int descent;
    };


    font_t*     font_alloc( char* the_font_path );

    void        font_dealloc( void* the_font );

    mtbmp_t* 	font_render_text( int width , int height , mtstr_t* string , font_t* the_font , textstyle_t text , glyphmetrics_t* glyphmetrics , mtvec_t* selections );


    #endif
