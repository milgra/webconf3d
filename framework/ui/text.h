

    #ifndef text_h
    #define text_h


    #include "../tools/math2.h"

    #define kTextAlignLeft      0
    #define kTextAlignCenter    1
    #define kTextAlignRight     2
    #define kTextAlignJustify   3


    typedef struct _glyphmetrics_t
    {
        float x;
        float y;

        float xoff;
        float yoff;
        float width;
        float kerning;

        uint32_t codepoint;
        uint32_t row;
        uint8_t  selected;
    
    } glyphmetrics_t;


    glyphmetrics_t glyphmetrics_init( uint32_t codepoint , float width , float height , int xoff , int yoff , int kerning , uint8_t selected );


    typedef struct _textstyle_t
    {
        char align;
        char editable;
        char selectable;
        char multiline;
        char autosize;
        char uppercase;
        
        float textsize;
        float marginsize;
        float cursorsize;
        
        uint32_t textcolor;
        uint32_t backcolor;

    } textstyle_t;


    void text_align( glyphmetrics_t* glyphs , textstyle_t style , vector2_t* dimensions , uint32_t count , int asc , int desc , int gap );


    typedef struct _textselection_t
    {
        uint32_t startindex;
        uint32_t endindex;
        uint32_t textcolor;
        uint32_t backcolor;        

    } textselection_t;


    #endif
