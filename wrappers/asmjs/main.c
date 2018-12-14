	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
	#include <sys/time.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <emscripten/html5.h>
	#include <emscripten/emscripten.h>
	#include "core/string.h"
	#include "view/input.h"
	#include "controller.h"

	controller_t* controller;

	int fullscreen;
	int screenwidth;
	int screenheight;

    long framecount = 0;
    long frametoresize = 0;
    long lasttouchsend;
    long lastscrollsend = 0;

	float pixelRatio = 1.0;
	float lastScrollX = 0.0;
	float lastScrollY = 0.0;

	input_t timerinput = { .type = kInputTypeTimer };
	input_t renderinput = { .type = kInputTypeRender };

	/* returns string value of dom path */

	char* getvalue( char* location , unsigned long length )
	{
        printf( "GETVALUE %s\n" , location );
		size_t pointer = EM_ASM_INT(
		{
		  var string = Pointer_stringify( $0 , $1 );
		  var result = eval(string);
		  var memoryArea = _malloc(result.length);
		  writeStringToMemory( result, memoryArea );
		  return memoryArea;
		}, location, length );
		
		return (char*)pointer;	
	}

    /* adds html5 video tag to the DOM model and plays it and renders it to texture */

    void playvideo( char* path , float volume , char loop , int texture )
    {
        EM_ASM_INT(
        {
          var path = "media/" + Pointer_stringify( $0 , $1 );
          var volume = $2;
          var loop = $3;
          var texture = $4;
          var video = Module[ "video" + texture ]; 

          Module.print('Creating player for texture ' + texture + ' path ' + path );

          if ( Module[ "playingvideos" ] == null ) Module[ "playingvideos" ] = new Array( );
          
          if ( video == null )
          {
             video = document.createElement('video');
             video.texture = texture;
             video.preload = "auto";
             video.src = path;

             video.onplay = function() 
             {
             };

             video.onended = function() 
             {
                video.added = false;
                var index = Module["playingvideos"].indexOf( video );
                if ( index > -1 ) Module["playingvideos"].splice(index, 1);
             };

             video.ontimeupdate = function() 
             {
                if ( !video.added )
                {
                    Module["playingvideos"].push( video );
                    video.added = true;					
                }
             };

             Module[ "video" + texture ] = video;
          }
          else 
          {
            if ( video.src != path ) 
            {
                video.pause();
                video.src = path;
            }
          }
          
          video.volume = volume;
          video.loop = loop;
          
          if ( volume > 0.0 ) video.play();
          
          return 0;
        }, path , strlen( path ), volume , loop , texture );
    }

    /* stops html5 video */

    void stopvideo( char* path , int texture )
    {
        EM_ASM_INT(
        {
          var path = "media/" + Pointer_stringify( $0 , $1 );
          var video = Module[ "video" + texture ];

          Module.print('Stopping player for texture ' + texture + ' path ' + path );

          if ( video != null )
          {
            video.pause();
            video.src = "";
            video.load();
          }
          
          return 0;
        }, path , strlen( path ), texture );
    }

    /* starts html5 audio */

    void playaudio( char* path , float volume , char loop )
    {
        EM_ASM_INT(
        {
          var path = "media/" + Pointer_stringify( $0 , $1 );
          var volume = $2;
          var loop = $3;
          var audio = Module[ path ];

          if ( audio == null )	
          {
              audio = new Audio( path );
              audio.preload = "auto";
              Module[ path ] = audio;
          }
          audio.volume = volume;
          audio.loop = loop;
          audio.play();
          return 0;
        }, path , strlen( path ), volume , loop );
    }

    /* stops html5 audio */

    void stopaudio( char* path )
    {
        EM_ASM_INT(
        {
          var path = "media/" + Pointer_stringify( $0 , $1 );
          
          if ( Module[ path ] != null )	Module[ path ].pause();
          return 0;
        }, path , strlen( path ) );
    }

    /* sets html5 audio volume */

    void setaudiovolume( char* path , float volume , char loop )
    {
        EM_ASM_INT(
        {
          var path = "media/" + Pointer_stringify( $0 , $1 );
          var volume = $2;
          
          if ( Module[ path ] != null )	Module[path].volume = $2;
          return 0;
        }, path , strlen( path ), volume );
    }

	/* obfuscates string */
	
//	void obfuscate( char* string )
//	{
//		char* valuerand = malloc( strlen(string) * 4 );		
//		int counter = 0;
//		for ( int index = 0 ; index < strlen(string) ; index++ )
//		{
//			valuerand[counter++] = string[index];
//			valuerand[counter++] = 40 + rand() % 61;
//			valuerand[counter++] = 40 + rand() % 61;
//			valuerand[counter++] = 40 + rand() % 61;
//		}
//		printf( "");
//		for ( int index = 0 ; index < strlen(valuerand) * 4 ; index++ ) printf( "%c" , valuerand[index] );
//	}

	/* deobfuscates hidden string */
	
	char* deobfuscate( char* string )
	{
		char* valuerand = calloc( 1 , strlen(string) / 4 + 1 );
		int counter = 0;
		for ( int index = 0 ; index < strlen(string) ; index+=4 ) valuerand[counter++] = string[index];
		return valuerand;
	}

	/* canvas size changed, setting resize timeout frame, reset fps check */

	EM_BOOL on_canvassize_changed(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
	{		
		emscripten_get_canvas_size( &screenwidth, &screenheight, &fullscreen );
        frametoresize = framecount + 30;
        controller_input( controller , renderinput );
        
		return 0;
	}

	/* key press event */

	EM_BOOL on_keypress(int eventType, const EmscriptenKeyboardEvent *event, void *userData)
	{
		int type = kInputTypeKeyPress;
        string_t* string = string_alloc( );
        string_addcodepoint( string , ( uint32_t ) event->keyCode );
        char* chars = string_bytes( string );
        
		if      ( event->keyCode == 32  ) controller_input( controller , ( input_t ) { .type = type , .stringa = chars , .key = kInputKeyTypeSpace } );
		else if ( event->keyCode == 8	) controller_input( controller , ( input_t ) { .type = type , .stringa = chars , .key = kInputKeyTypeBackspace } );
		else if ( event->keyCode == 13	) controller_input( controller , ( input_t ) { .type = type , .stringa = chars , .key = kInputKeyTypeReturn } );
		else if ( event->keyCode == 10	) controller_input( controller , ( input_t ) { .type = type , .stringa = chars , .key = kInputKeyTypeReturn } );
		else controller_input( controller , ( input_t ) { .type = type , .stringa = chars , .key = kInputKeyTypeNormal } );
        
		mem_release( chars );
        mem_release( string );

		return 1;
	}

	/* key up/down event */

	EM_BOOL on_key(int eventType, const EmscriptenKeyboardEvent *event, void *userData)
	{
		int type = kInputTypeKeyPress;
		if ( eventType == EMSCRIPTEN_EVENT_KEYUP ) type = kInputTypeKeyRelease;

		if		( event->keyCode == 37  ) controller_input( controller , ( input_t ) { .type = type , .stringa = NULL , .key = kInputKeyTypeLeftArrow } );
		else if ( event->keyCode == 38  ) controller_input( controller , ( input_t ) { .type = type , .stringa = NULL , .key = kInputKeyTypeUpArrow } );
		else if ( event->keyCode == 39  ) controller_input( controller , ( input_t ) { .type = type , .stringa = NULL , .key = kInputKeyTypeRightArrow } );
		else if ( event->keyCode == 40  ) controller_input( controller , ( input_t ) { .type = type , .stringa = NULL , .key = kInputKeyTypeDownArrow } );
		else if ( event->keyCode == 16	) {  } // SHIFT
		else if ( event->keyCode == 17	) {  } // CTRL
		else if ( event->keyCode == 18	)
        {
            if ( type == kInputTypeKeyRelease )
            {
                controller_input( controller , ( input_t ) { .type = kInputTypeScrollEnd } );            
            }
        } // ALT
		else if ( event->keyCode == 20	) {  } // CAPS
		else if ( event->keyCode == 91	) {  } // WIN CMD
		else if ( event->keyCode == 9	) {  } // TAB
		else if ( event->keyCode == 27	) {  } // ESC

		if (eventType == EMSCRIPTEN_EVENT_KEYUP && event->which == 27)
		{
			EmscriptenFullscreenChangeEvent fsce;
			EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fsce);
			if (!fsce.isFullscreen) {
				printf("Requesting fullscreen..\n");
				ret = emscripten_request_fullscreen(0, 1);
			} 
			else 
			{
				printf("Exiting fullscreen..\n");
				ret = emscripten_exit_fullscreen();
				ret = emscripten_get_fullscreen_status(&fsce);
			}
		}

		return 0;
	}

	/* mouse event */
	
	char dragged = 0;

	EM_BOOL on_mouse(int eventType, const EmscriptenMouseEvent* event, void *userData)
	{
		int type = 0;
		if ( eventType == EMSCRIPTEN_EVENT_MOUSEDOWN ) 
		{
			type = kInputTypeTouchDown;
			dragged = 1;
		}
		else if ( eventType == EMSCRIPTEN_EVENT_MOUSEUP ) 
		{
			type = kInputTypeTouchUp;
			dragged = 0;
		}
		else if ( eventType == EMSCRIPTEN_EVENT_MOUSEMOVE ) 
		{
            //if ( framecount < lasttouchsend + 1 ) return 1;
			type = kInputTypeTouchDrag;
			if ( dragged == 0 ) return 1;
		}

        lasttouchsend = framecount;
        
        printf( "MOUSE %f\n", (float) ( event->canvasY ) * pixelRatio );
				
		controller_input( controller , ( input_t ) { .type = type , .stringa = "mouse" , .floata = (float) event->canvasX * pixelRatio , .floatb = (float) ( event->canvasY ) * pixelRatio } );

		return 1;
	}

    /* touch event */

	EM_BOOL on_touch(int eventType, const EmscriptenTouchEvent* event, void *userData)
	{
		int type = 0;
		if ( eventType == EMSCRIPTEN_EVENT_TOUCHSTART )
		{
			type = kInputTypeTouchDown;
		}
		else if ( eventType == EMSCRIPTEN_EVENT_TOUCHMOVE )
		{
			type = kInputTypeTouchDrag;
		}
		else if ( eventType == EMSCRIPTEN_EVENT_TOUCHEND )
		{
			type = kInputTypeTouchUp;
		}
		else if ( eventType == EMSCRIPTEN_EVENT_TOUCHCANCEL )
		{
			type = kInputTypeTouchUp;
		}
		
		for ( int index = 0 ; index < event->numTouches ; index++ )
		{
			EmscriptenTouchPoint point = event->touches[ index ];
			char buffer[20];
			long mynumber = point.identifier + 1;
			sprintf(buffer, "%li" , point.identifier );
			if ( point.isChanged == 1 )	controller_input( controller , ( input_t ) { .type = type , .stringa = buffer , .floata = (float) point.canvasX  * pixelRatio, .floatb = (float) ( point.canvasY ) * pixelRatio } );
		}

		return 1;
	}

    /* wheel event */
	
	EM_BOOL on_wheel(int eventType, const EmscriptenWheelEvent* event, void *userData)
	{
		if ( eventType == EMSCRIPTEN_EVENT_WHEEL )
		{
            char alt = event->mouse.altKey;
			if ( fabs(lastScrollX) < 2.0 || fabs(lastScrollY) < 2.0 )
			{
				controller_input( controller , ( input_t ) { .type = kInputTypeScrollStart , .floata = (float) event->mouse.canvasX  * pixelRatio, .floatb = (float) ( -event->mouse.canvasY ) * pixelRatio , .chara = alt } );
			}
			
			controller_input( controller , ( input_t ) { .type = kInputTypeScroll , .floata = (float)event->deltaX , .floatb = (float)-event->deltaY , .chara = alt  } );

//			if ( fabs(event->deltaX ) < 2.0 && fabs(event->deltaY ) < 2.0 )
//			{
//				controller_input( controller , ( input_t ) { .type = kInputTypeScrollEnd , .floata = (float)event->deltaX , .floatb = (float)-event->deltaY , .chara = alt  } );
//			}
			
			lastScrollX = event->deltaX;
			lastScrollY = event->deltaY;
            
            lastscrollsend = framecount;
		}
		return 1;
	}

	/* emscripten main loop */

	void on_main_loop( )
	{
        /* check resize, send resize event and initiate fps checking */
        
        //if ( dragged == 1 ) return;
        
        if ( frametoresize > 0 && frametoresize == framecount )
        {
            printf( "sending resize\n" );
            controller_input( controller , ( input_t ) {.type = kInputTypeResize , .floata = screenwidth , .floatb = screenheight } );
            
            frametoresize = 0;

            int fps = 0;
            //if ( screenwidth > 1400 ) fps = 45;
            //if ( screenwidth > 1600 ) fps = 30;
            emscripten_cancel_main_loop( );
            emscripten_set_main_loop( on_main_loop, fps, 0 );
        }

        /* copy active videos to textures */

		EM_ASM_INT(
		{
		  if ( Module[ "playingvideos" ] != null && Module["playingvideos"].length > 0 )	
		  {
			Module[ "playingvideos" ].forEach( function (video, index, array) 
			{
				_glBindTexture(GLctx.TEXTURE_2D, video.texture);
				//GLctx.pixelStorei(GLctx.UNPACK_FLIP_Y_WEBGL, true);
				GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA,GLctx.UNSIGNED_BYTE, video);
				//GLctx.pixelStorei(GLctx.UNPACK_FLIP_Y_WEBGL, false);
			});
		  }
		  return 0;
		},0);
        
        /* check for scroll end */
        
        if ( lastscrollsend > 0 )
        {
            if ( framecount == lastscrollsend + 20 )
            {
                lastscrollsend = 0;
                controller_input( controller , ( input_t ) { .type = kInputTypeScrollEnd  } );
            }
        }

        controller_input( controller , timerinput );

		framecount++;

        /* check framecount overflow */
        
        if ( framecount == INT32_MAX )
        {
            if ( frametoresize > 0 ) frametoresize -= framecount;
            framecount = 0;
        }
	}
	
	/* entering point */
	
	int main( void )
	{
		/* create webgl context on canvas */
		
		EmscriptenWebGLContextAttributes attributes;
		emscripten_webgl_init_context_attributes( &attributes );

		attributes.alpha = 0;
		attributes.depth = 0;
		attributes.stencil = 0;
		attributes.antialias = 0;
		attributes.premultipliedAlpha = 0;
		attributes.preserveDrawingBuffer = 0;
		attributes.enableExtensionsByDefault = 0;
  
        EMSCRIPTEN_RESULT result;
  
		assert(emscripten_webgl_get_current_context() == 0);
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context( 0, &attributes ); assert( context > 0 );
		result = emscripten_webgl_make_context_current(context); assert( result == EMSCRIPTEN_RESULT_SUCCESS );
		assert(emscripten_webgl_get_current_context() == context);
		
		emscripten_get_canvas_size( &screenwidth, &screenheight, &fullscreen );
		pixelRatio = EM_ASM_INT( { return window.pixelRatio; }, 0 );
        
        environment_t env =
        {
            .width = 800 ,
            .height = 600 ,
            .scale = pixelRatio ,
            .windowed = 0 ,
            .path_library = string_copycstring( "IDBFS" ) ,
            .path_resources = string_copycstring( "resources" ) ,
            .defaultFrameBuffer = 0 ,
            .defaultRenderBuffer = 0
        };
		
		controller = controller_alloc( env );
		controller_input( controller , ( input_t ) {.type = kInputTypeResize , .floata = screenwidth , .floatb = screenheight } );

		emscripten_set_resize_callback( 0, 0, 1, on_canvassize_changed );
		emscripten_set_keypress_callback (0, 0, 1, on_keypress );
		emscripten_set_keydown_callback (0, 0, 1, on_key );
		emscripten_set_keyup_callback( 0, 0, 1, on_key );
		emscripten_set_mouseup_callback(0, 0, 1, on_mouse);
		emscripten_set_mousedown_callback(0, 0, 1, on_mouse);
		emscripten_set_mousemove_callback(0, 0, 1, on_mouse);
		emscripten_set_touchstart_callback(0, 0, 1, on_touch);
		emscripten_set_touchend_callback(0, 0, 1, on_touch);
		emscripten_set_touchmove_callback(0, 0, 1, on_touch);
		emscripten_set_touchcancel_callback(0, 0, 1, on_touch);
		emscripten_set_wheel_callback(0, 0, 1, on_wheel);
        
        /* domain check */
  
		char* obfpath = "w(@\\iR;+nFX*dQ,=o<<`wJ^V.O4Ald82o-(Vc?;Ja?QXt`;Ci/K8o)NJnEKM.CEShSKVo?DAs?c>tcM`n0Aba`BOmP6Qe9X[";
		char* obfwant = "m)BGi?NKld(8g\\R,r]N*aEG2.5JHc\\L/o7*_mB[@";		
		//char* path = "window.location.hostname";
		//char* want = "milgra.com";
        //obfuscate( path ); obfuscate( want );
		char* path = deobfuscate( obfpath );
		char* want = deobfuscate( obfwant );
		char* domain = getvalue( path , strlen(path) );
		
		if ( strstr(domain,want) == NULL && strlen( domain ) > 0 ) controller_input( controller , ( input_t ) {.type = kInputTypeDomainError } );

		free( path );
		free( want );
		free( domain );
        
        /* start main loop with full throttle, fps check will be initiated */
        
        int fps = 0;
//        if ( screenwidth > 1400 ) fps = 45;
//        if ( screenwidth > 1600 ) fps = 30;

		emscripten_set_main_loop( on_main_loop, 0, 0 );

		return 0;
	}
	
	/* input from controller */

	void* controller_output( input_t input )
	{
		if ( strcmp( input.name , "flushbuffer" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "showkeyboard" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "hidekeyboard" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "dragwindow" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "dropwindow" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "closewindow" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "maximizewindow" ) == 0 )
		{
			EmscriptenFullscreenChangeEvent fsce;
			
			EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fsce);
			if (!fsce.isFullscreen) {
				ret = emscripten_request_fullscreen(0, 1);
			} 
			else 
			{
				ret = emscripten_exit_fullscreen();
				ret = emscripten_get_fullscreen_status(&fsce);
			}
		}
		else if ( strcmp( input.name , "playvideo" ) == 0 )
		{
            playvideo( input.stringa , input.floata , input.chara , input.inta );
		}
		else if ( strcmp( input.name , "stopvideo" ) == 0 )
		{
            stopvideo( input.stringa , input.inta );
		}
		else if ( strcmp( input.name , "playaudio" ) == 0 )
		{
            playaudio( input.stringa , input.floata , input.chara );
		}
		else if ( strcmp( input.name , "stopaudio" ) == 0 )
		{
            stopaudio( input.stringa );
		}
		else if ( strcmp( input.name , "setaudiovolume" ) == 0 )
		{
            setaudiovolume( input.stringa , input.floata , input.chara );
		}
		else if ( strcmp( input.name , "setclipboard" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "getclipboard" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "showad" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "rateapp" ) == 0 )
		{
		}
		else if ( strcmp( input.name , "flushfs" ) == 0 )
		{
            printf( "Settings flushed\n" );
			EM_ASM_INT(
			{
				FS.syncfs(false, function (err) { assert(!err);	});
				return 0;
			},NULL);
		}
		else if ( strcmp( input.name , "checkadblock" ) == 0 )
		{
			int result = EM_ASM_INT(
			{
				FS.syncfs(false, function (err) { assert(!err);	});
				return window.initerror;
			},NULL);
            printf( "ADBLOCK DETECTED : %i\n" , result );
		}
		else if ( strcmp( input.name , "openurl" ) == 0 )
		{
			EM_ASM_INT(
			{
				window.open(Pointer_stringify( $0 , $1 ),"_blank");
				return 0;
			}, input.stringa, strlen( input.stringa ) );
		}
		
		return NULL;
	}
