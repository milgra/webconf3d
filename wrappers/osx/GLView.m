//
//  GLView.m
//  minimal
//
//  Created by Milan Toth on 2017. 01. 06..
//  Copyright © 2017. milgra. All rights reserved.
//

    #define CONTROLLER_IMPLEMENTATION
    #import "controller.h"
    #undef CONTROLLER_IMPLEMENTATION

    #import "GLView.h"
    #import "MediaHandler.h"
    #import "AppDelegate.h"
    #import "mtcstr.h"

	static GLView* staticView;
    static AppDelegate* staticDelegate;
    static controller_t* staticController;
    static MediaHandler* staticMediaHandler;


	@implementation GLView
	{
		BOOL dragged;
        float scale;
        controller_t* controller;
        
		NSTimer* timer;
		NSPoint last;
		NSPoint position;
        NSRect oldRect;
	}


	- ( id ) initWithFrame : ( NSRect ) frameRect delegate : ( id ) delegate
	{
        staticDelegate = delegate;
        
        NSOpenGLPixelFormatAttribute attributes[ ] = { NSOpenGLPFADoubleBuffer , kCGLPFADepthSize, 16, NSOpenGLPFAOpenGLProfile , NSOpenGLProfileVersionLegacy , 0 };
		NSOpenGLPixelFormat* pixelFormat = [ [ NSOpenGLPixelFormat alloc ] initWithAttributes : attributes ];
        
		self = [ super initWithFrame : frameRect pixelFormat : pixelFormat ];
		if ( self )
		{
			[ self setAutoresizingMask : NSViewWidthSizable | NSViewHeightSizable ];
			[ self setAcceptsTouchEvents : YES ];
			[ self setWantsBestResolutionOpenGLSurface : YES ];
			[ self.openGLContext makeCurrentContext ];
            
            staticView = self;
		}
		return self;
	}
	

	- ( void ) load
	{
        staticMediaHandler = [ [ MediaHandler alloc ] init ];

		NSString* resPath = [ [ NSBundle mainBundle ] resourcePath ];
		NSString* libPath = [ NSSearchPathForDirectoriesInDomains( NSLibraryDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ];
		
		scale = [ [ self window ] backingScaleFactor ];
        
        environment_t env =
        {
            .width = self.frame.size.width * scale ,
            .height = self.frame.size.height * scale ,
            .scale = scale ,
            .windowed = 1 ,
            .path_library = mtcstr_fromcstring( (char*) [ libPath cStringUsingEncoding : NSUTF8StringEncoding ] ) ,
            .path_resources = mtcstr_fromcstring( (char*) [ resPath cStringUsingEncoding : NSUTF8StringEncoding ] ) ,
            .defaultFrameBuffer = 0 ,
            .defaultRenderBuffer = 0
        };
		
		controller = controller_alloc( env );
        
        staticController = controller;
        
        [ self.openGLContext flushBuffer ];
        
        oldRect = self.frame;
		
		timer = [ NSTimer scheduledTimerWithTimeInterval : 1.0 / 60.0
						  target : self
						  selector : @selector( timerEvent )
						  userInfo : nil
						  repeats : YES ];
	}
	

	- ( void ) mouseDown : ( NSEvent* ) theEvent
	{
		NSRect backingBounds = [self convertRectToBacking:[self bounds]];
		NSPoint location = [ theEvent locationInWindow ];

        input_t input = { 0 };
        input.type = kInputTypeTouchDown;
        input.stringa = "mouse";
        input.floata = location.x * scale;
        input.floatb = backingBounds.size.height - location.y * scale;

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];

		last = [ NSEvent mouseLocation ];
		position = self.window.frame.origin;
	}


	- ( void ) mouseUp : ( NSEvent* ) theEvent
	{
		NSRect backingBounds = [self convertRectToBacking:[self bounds]];
		NSPoint location = [ theEvent locationInWindow ];

        input_t input = { 0 };
        input.type = kInputTypeTouchUp;
        input.stringa = "mouse";
        input.floata = location.x * scale;
        input.floatb = backingBounds.size.height - location.y * scale;

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
	}


	- ( void ) mouseMoved : ( NSEvent* ) theEvent
	{
		NSRect backingBounds = [self convertRectToBacking:[self bounds]];
		NSPoint location = [ theEvent locationInWindow ];

        input_t input = { 0 };
        input.type = kInputTypeMouseMove;
        input.stringa = "mouse";
        input.floata = location.x * scale;
        input.floatb = backingBounds.size.height - location.y * scale;

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }


	- ( void ) mouseDragged : ( NSEvent* ) theEvent
	{
		NSRect backingBounds = [self convertRectToBacking:[self bounds]];
		NSPoint location = [ theEvent locationInWindow ];
		NSPoint mouse = [ NSEvent mouseLocation ];
		NSPoint delta = NSMakePoint( mouse.x - last.x , mouse.y - last.y );

        input_t input = { 0 };
        input.type = kInputTypeTouchDrag;
        input.stringa = "mouse";
        input.floata = location.x * scale;
        input.floatb = backingBounds.size.height - location.y * scale;

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
		
		if ( dragged )
		{
			NSRect windowFrame = [self.window frame];
			NSPoint newOrigin = windowFrame.origin;

			newOrigin.x = position.x + delta.x;
			newOrigin.y = position.y + delta.y;

			[self.window setFrameOrigin:newOrigin];
		}
	}
	

    - ( void ) keyDown : ( NSEvent* ) theEvent
    {
		NSUInteger flags = [ theEvent modifierFlags ] & NSEventModifierFlagDeviceIndependentFlagsMask;

        input_t input = { 0 };
		
		if( flags == NSEventModifierFlagCommand )
		{
			/* CTRL C */
			if ( [theEvent keyCode] == 8 )
			{
//				mtstr_t* selected = controller_input( controller , (input_t){.type = kInputTypeGetSelected } );
//                char* selectedc = mtstr_bytes( selected );
//				NSPasteboard*  myPasteboard  = [NSPasteboard generalPasteboard];
//				[myPasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:self];
//				[myPasteboard setString : [ NSString stringWithUTF8String : selectedc ] forType:NSPasteboardTypeString];
//                mtmem_release( selectedc );
			}
			/* CTRL V */
			else if ( [theEvent keyCode] == 9 )
			{
//				NSPasteboard*  myPasteboard  = [NSPasteboard generalPasteboard];
//				NSString* myString = [myPasteboard  stringForType:NSPasteboardTypeString];
//				char* chars = ( char* ) [ myString cStringUsingEncoding : NSUTF8StringEncoding ];
//				controller_input( controller , ( input_t ) { .type = kInputTypeKeyPress , .stringa = chars , .key = kInputKeyTypeNormal } );
			}
		}
		else
		{
			char* chars = ( char* ) [ theEvent.characters cStringUsingEncoding : NSUTF8StringEncoding ];
            
            input.type = kInputTypeKeyPress;
            input.stringa = chars;
			
			if		( theEvent.keyCode == 123 ) input.key = kInputKeyTypeLeftArrow;
			else if ( theEvent.keyCode == 124 ) input.key = kInputKeyTypeRightArrow;
			else if ( theEvent.keyCode == 126 ) input.key = kInputKeyTypeUpArrow;
			else if ( theEvent.keyCode == 125 ) input.key = kInputKeyTypeDownArrow;
			else if ( theEvent.keyCode == 49  ) input.key = kInputKeyTypeSpace;
			else if ( chars[0] == 127		  ) input.key = kInputKeyTypeBackspace;
			else if ( chars[0] == 13		  ) input.key = kInputKeyTypeReturn;
			else if ( chars[0] == 10		  ) input.key = kInputKeyTypeReturn;
			else input.key = kInputKeyTypeNormal;
		}

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }
    
	/* key up eveng */
	
    - ( void ) keyUp : ( NSEvent* ) theEvent
    {
		char* chars = ( char* ) [ theEvent.characters cStringUsingEncoding : NSUTF8StringEncoding ];

        input_t input = { 0 };

        input.type = kInputTypeKeyRelease;
        input.stringa = chars;
		
        if		( theEvent.keyCode == 123 ) input.key = kInputKeyTypeLeftArrow;
        else if ( theEvent.keyCode == 124 ) input.key = kInputKeyTypeRightArrow;
        else if ( theEvent.keyCode == 126 ) input.key = kInputKeyTypeUpArrow;
        else if ( theEvent.keyCode == 125 ) input.key = kInputKeyTypeDownArrow;
        else if ( theEvent.keyCode == 49  ) input.key = kInputKeyTypeSpace;
        else if ( chars[0] == 127		  ) input.key = kInputKeyTypeBackspace;
        else if ( chars[0] == 13		  ) input.key = kInputKeyTypeReturn;
        else if ( chars[0] == 10		  ) input.key = kInputKeyTypeReturn;
        else input.key = kInputKeyTypeNormal;

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }

    char needsscrollend = 0;


    - ( void ) scrollEnd
    {
        if ( needsscrollend == 1 )
        {
            input_t input = { 0 };
            controller_input( controller , &input );
            if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
        }
    }
	

	- ( void ) scrollWheel : ( NSEvent* ) event
	{
        input_t input = { 0 };

		NSRect backingBounds = [ self convertRectToBacking : [ self bounds ] ];

		/* normal scroll events */
		
		if ( event.phase == NSEventPhaseMayBegin )
		{
            input.type = kInputTypeScroll;
            input.floata = 0.0;
            input.floatb = 0.0;
		}
		else if ( event.phase == NSEventPhaseBegan )
		{
            input.type = kInputTypeScrollStart;
            input.floata = event.locationInWindow.x * scale;
            input.floatb = backingBounds.size.height - event.locationInWindow.y * scale;
		}
		else if ( event.phase == NSEventPhaseChanged )
		{
            input.type = kInputTypeScroll;
            input.floata = event.scrollingDeltaX;
            input.floatb = event.scrollingDeltaY;
		}
		else if ( event.phase == NSEventPhaseEnded || 
				  event.phase == NSEventPhaseCancelled )
		{
            needsscrollend = 1;
            [self performSelector:@selector(scrollEnd)
                       withObject:nil
                       afterDelay:0.01];
		}

		if ( event.momentumPhase == NSEventPhaseBegan )
		{
            needsscrollend = 0;
        }
		else if ( event.momentumPhase == NSEventPhaseChanged )
		{
			if ( fabs(event.deltaX) > 0.5 || fabs(event.deltaY) > 0.5 ) 
			{
                input.type = kInputTypeScroll;
                input.floata = event.scrollingDeltaX;
                input.floatb = event.scrollingDeltaY;
			}
		}
		else if ( event.momentumPhase == NSEventPhaseEnded ||
				  event.momentumPhase == NSEventPhaseCancelled )
		{
            input.type = kInputTypeScrollEnd;
		}
        
        controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
	}
	

    - ( void ) magnifyWithEvent : ( NSEvent* ) event
    {
        input_t input = { 0 };

		NSRect backingBounds = [self convertRectToBacking:[self bounds]];
		if ( event.phase == NSEventPhaseBegan )
		{
            input.type = kInputTypeZoomStart;
            input.floata = event.locationInWindow.x * scale;
            input.floatb = backingBounds.size.height - event.locationInWindow.y * scale;
		}
		else if ( event.phase == NSEventPhaseChanged )
		{
            input.type = kInputTypeZoom;
            input.floata = event.magnification;
            input.floatb = event.magnification;
		}
		else if ( event.phase == NSEventPhaseEnded || event.phase == NSEventPhaseCancelled )
		{
            input.type = kInputTypeZoomEnd;
            input.floata = event.locationInWindow.x * scale;
            input.floatb = backingBounds.size.height - event.locationInWindow.y * scale;
            
		}

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }


	- ( void ) update
	{
        [ self.openGLContext update ];

        input_t input = { 0 };

        input.type = kInputTypeResize;
        input.floata = self.frame.size.width * scale;
        input.floatb = self.frame.size.height * scale;
        
        controller_input( controller , &input );
        
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
	}


	- ( void ) render
    {
        input_t input = { 0 };
        input.type = kInputTypeRender;
        
        controller_input( controller , &input );
        
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }


	- ( void ) timerEvent
    {
        BOOL dorender = [staticMediaHandler timerEvent:controller];
        
        input_t input = { 0 };
        input.type = kInputTypeTimer;
        input.render = dorender ? 1 : 0;
        controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }


	- ( void ) startDrag
    {
        dragged = YES;
    }


	- ( void ) stopDrag
    {
        dragged = NO;
    }


	- ( BOOL ) canBecomeFirstResponder
    {
        return YES;
    }


	- ( BOOL ) recognizeTwoFingerGestures
    {
        return YES;
    }


    - ( void ) pushNotificationTokenReceived : ( NSString* ) token
    {
        input_t input = { 0 };
        input.type = kInputTypePushTokenArrived;
        input.stringb = "macos";
        input.stringa = ( char* )[token cStringUsingEncoding:NSASCIIStringEncoding];

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self.openGLContext flushBuffer ];
    }


    - ( void ) copy : ( char* ) string
    {
        NSPasteboard*  myPasteboard  = [NSPasteboard generalPasteboard];
        [myPasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];
        [myPasteboard setString : [ NSString stringWithUTF8String : string ] forType:NSPasteboardTypeString];
    }


    - ( void ) paste
    {
        NSPasteboard*  myPasteboard  = [NSPasteboard generalPasteboard];
        NSString* myString = [myPasteboard  stringForType:NSPasteboardTypeString];
        char* chars = ( char* ) [ myString cStringUsingEncoding : NSUTF8StringEncoding ];
        input_t input = { 0 };
        input.type = kInputTypePaste;
        input.stringa = chars;
        controller_input( staticController , &input );
    }


	@end

	
	void controller_output( input_t* input )
	{
		if ( strcmp( input->name , "flushbuffer" ) != 0 )
		{
		
			printf( "output %s %s\n" , input->name , input->stringa );
		}
	
		if      ( strcmp( input->name , "dragwindow"        ) == 0 ) [ staticView startDrag ];
		else if ( strcmp( input->name , "dropwindow"        ) == 0 ) [ staticView stopDrag ];
		else if ( strcmp( input->name , "closewindow"       ) == 0 ) [ staticDelegate closeWindow ];
		else if ( strcmp( input->name , "maximizewindow"    ) == 0 ) [ staticDelegate maximizeWindow ];
		else if ( strcmp( input->name , "copy"              ) == 0 ) [ staticView copy : input->stringa ];
		else if ( strcmp( input->name , "paste"             ) == 0 ) [ staticView paste ];
		else if ( strcmp( input->name , "openurl"           ) == 0 ) [ [ NSWorkspace sharedWorkspace ] openURL : [ NSURL URLWithString : [ NSString stringWithUTF8String : input->stringa ] ] ];
        
        /* media related */
        
		else if ( strcmp( input->name , "loadvideo"         ) == 0 ) [ staticMediaHandler loadMedia : staticController input : input ];
		else if ( strcmp( input->name , "playvideo"         ) == 0 ) [ staticMediaHandler playMedia : staticController input : input ];
		else if ( strcmp( input->name , "stopvideo"         ) == 0 ) [ staticMediaHandler stopMedia : staticController input : input ];
		else if ( strcmp( input->name , "selectmedia"       ) == 0 ) [ staticMediaHandler selectMedia: staticController input : input window : [staticView window] ];
		else if ( strcmp( input->name , "uploadmedia"       ) == 0 ) [ staticMediaHandler uploadMedia: staticController input : input ];
	}
