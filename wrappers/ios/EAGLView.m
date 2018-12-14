//
//  EAGLView.m
//  chat
//
//  Created by Milan Toth on 2017. 01. 07..
//  Copyright © 2017. Milan Toth. All rights reserved.
//

    #import "EAGLView.h"
    #import "MediaHandler.h"
	#import <OpenGLES/EAGL.h>
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
	#import "controller.h"

    #ifdef ADMOB
    #import "AdHandler.h"
    #endif


    static EAGLView* staticView;
    static MediaHandler* staticMediaHandler;	
    static controller_t* staticController;
    #ifdef ADMOB
    static AdHandler* staticAdHandler;
    #endif

    UIViewController* vvc;

	@implementation EAGLView
	{
		GLint backingWidth;
		GLint backingHeight;		
		GLuint viewRenderbuffer;
		GLuint viewFramebuffer;	
		NSTimer* timer;
		EAGLContext *context;
        controller_t* controller;
	}


	+ (Class)layerClass
	{
		return [CAEAGLLayer class];
	}
	

	- ( id ) initWithFrame : ( CGRect ) frame
	{    
		if ( ( self = [ super initWithFrame : frame ] ) )
		{
			[ self setMultipleTouchEnabled : YES ];
            
            staticView = self;

			NSDictionary* properties = [ NSDictionary dictionaryWithObjectsAndKeys :
				[ NSNumber numberWithBool : YES ], 
				kEAGLDrawablePropertyRetainedBacking,
				kEAGLColorFormatRGBA8 , 
				kEAGLDrawablePropertyColorFormat ,
				nil ];

			CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
			eaglLayer.opaque = YES;
			eaglLayer.drawableProperties = properties; 
			
			context = [ [ EAGLContext alloc ] initWithAPI : kEAGLRenderingAPIOpenGLES2 ];

			[EAGLContext setCurrentContext:context];
			
			glGenFramebuffersOES( 1, &viewFramebuffer );
			glGenRenderbuffersOES( 1, &viewRenderbuffer );
			glBindFramebufferOES( GL_FRAMEBUFFER_OES, viewFramebuffer );
			glBindRenderbufferOES( GL_RENDERBUFFER_OES, viewRenderbuffer );
			glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer );

			[ [ NSNotificationCenter defaultCenter ] 
				addObserver : self
				selector : @selector(keyboardWillShow:)
				name : UIKeyboardWillShowNotification
				object : nil ];

			[ [ NSNotificationCenter defaultCenter ] 
				addObserver : self
				selector : @selector(keyboardWillHide:)
				name : UIKeyboardWillHideNotification
				object : nil ];

			self.contentScaleFactor = [ [ UIScreen mainScreen ] scale ];
		}
		return self;
	}
	

	- ( void ) dealloc
	{
		if ( [ EAGLContext currentContext ] == context ) [ EAGLContext setCurrentContext : nil ];
		
		context = nil;
		[ super dealloc ];
	}


	- ( void ) load
	{
        staticMediaHandler = [ [ MediaHandler alloc ] init ];
        
        #ifdef ADMOB
        staticAdHandler = [ [ AdHandler alloc ] init ];
        #endif

		NSString* resPath = [ [ NSBundle mainBundle ] resourcePath ];
		NSString* libPath = [ NSSearchPathForDirectoriesInDomains( NSLibraryDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ];
        
        float scale = [[ UIScreen mainScreen ] scale ];

        environment_t env =
        {
            .width = self.frame.size.width * scale ,
            .height = self.frame.size.height * scale ,
            .scale = scale ,
            .windowed = 0 ,
            .path_library = mtcstr_fromcstring( (char*) [ libPath cStringUsingEncoding : NSUTF8StringEncoding ] ) ,
            .path_resources = mtcstr_fromcstring( (char*) [ resPath cStringUsingEncoding : NSUTF8StringEncoding ] ) ,
            .defaultFrameBuffer = viewFramebuffer ,
            .defaultRenderBuffer = viewRenderbuffer
        };
		
		controller = controller_alloc( env );
        
        staticController = controller;

		timer = [ NSTimer 
			scheduledTimerWithTimeInterval : 1.0 / 60.0
			target : self
			selector : @selector(timerEvent)
			userInfo : nil
			repeats : YES ];
	}


    - ( void ) pushNotificationTokenReceived : ( NSString* ) token
    {
        input_t input = { 0 };
        input.type = kInputTypePushTokenArrived;
        input.stringb = "iphone";
        input.stringa = ( char* )[token cStringUsingEncoding:NSASCIIStringEncoding];

		controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
    }


	- ( void ) layoutSubviews
	{
		[ context renderbufferStorage : GL_RENDERBUFFER_OES fromDrawable : ( CAEAGLLayer* ) self.layer ];
		glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth );
		glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight );
		
        input_t input = { 0 };

        input.type = kInputTypeResize;
        input.floata = self.frame.size.width * [ [ UIScreen mainScreen ] scale ];
        input.floatb = self.frame.size.height * [ [ UIScreen mainScreen ] scale ];
        
        controller_input( controller , &input );
        
        if ( input.render == 1 ) [ self flushBuffer ];
			
		[ context presentRenderbuffer : GL_RENDERBUFFER_OES ];
	}


	- ( void ) touchesBegan : ( NSSet* ) touches withEvent : ( UIEvent* ) event
	{
		for ( UITouch *touch in touches )
		{
			CGPoint location = [ touch locationInView : self];

			float x = location.x * [ [ UIScreen mainScreen ] scale ];
			float y = location.y * [ [ UIScreen mainScreen ] scale ];

			NSValue *touchValue = [ NSValue valueWithPointer : (__bridge const void * _Nullable ) ( touch ) ];
			NSString *key = [ NSString stringWithFormat : @"%@", touchValue ];
			char* touchid = ( char* ) [ key cStringUsingEncoding : NSUTF8StringEncoding ];			

            input_t input = { 0 };
            input.type = kInputTypeTouchDown;
            input.stringa = touchid;
            input.floata = x;
            input.floatb = y;

            controller_input( controller , &input );
            if ( input.render == 1 ) [ self flushBuffer ];
		}
	}


	- ( void ) touchesMoved : ( NSSet* ) touches withEvent : ( UIEvent* ) event
	{
		for ( UITouch *touch in touches )
		{
			CGPoint previous = [ touch previousLocationInView : self ];
			CGPoint location = [ touch locationInView : self ];
			
			if ( location.x != previous.x || location.y != previous.y )
			{
				float x = location.x * [ [ UIScreen mainScreen ] scale ];
				float y = location.y * [ [ UIScreen mainScreen ] scale ];

				NSValue *touchValue = [ NSValue valueWithPointer : ( __bridge const void * _Nullable ) ( touch ) ];
				NSString *key = [ NSString stringWithFormat : @"%@", touchValue ];
				char* touchid = ( char* ) [ key cStringUsingEncoding : NSUTF8StringEncoding ];			

                input_t input = { 0 };
                input.type = kInputTypeTouchDrag;
                input.stringa = touchid;
                input.floata = x;
                input.floatb = y;

                controller_input( controller , &input );
                if ( input.render == 1 ) [ self flushBuffer ];
			}
		}	
	}


	- ( void ) touchesEnded : ( NSSet* ) touches withEvent : ( UIEvent* ) event
	{
		for ( UITouch *touch in touches )
		{
			CGPoint location = [ touch locationInView : self ];

			float x = location.x * [ [ UIScreen mainScreen ] scale ];
			float y = location.y * [ [ UIScreen mainScreen ] scale ];

			NSValue *touchValue = [ NSValue valueWithPointer : ( __bridge const void * _Nullable ) ( touch ) ];
			NSString *key = [ NSString stringWithFormat : @"%@", touchValue ];
			char* touchid = ( char* ) [ key cStringUsingEncoding : NSUTF8StringEncoding ];			

            input_t input = { 0 };
            input.type = kInputTypeTouchUp;
            input.stringa = touchid;
            input.floata = x;
            input.floatb = y;

            controller_input( controller , &input );
            if ( input.render == 1 ) [ self flushBuffer ];
		}
	}


	- ( void ) touchesCancelled : ( NSSet* ) touches withEvent : ( nullable UIEvent* ) event
	{
		for ( UITouch *touch in touches )
		{
			CGPoint location = [ touch locationInView : self ];

			float x = location.x * [ [ UIScreen mainScreen ] scale ];
			float y = location.y * [ [ UIScreen mainScreen ] scale ];

			NSValue *touchValue = [ NSValue valueWithPointer : ( __bridge const void * _Nullable ) ( touch ) ];
			NSString *key = [ NSString stringWithFormat : @"%@", touchValue ];
			char* touchid = ( char* ) [ key cStringUsingEncoding : NSUTF8StringEncoding ];			

            input_t input = { 0 };
            input.type = kInputTypeTouchUp;
            input.stringa = touchid;
            input.floata = x;
            input.floatb = y;

            controller_input( controller , &input );
            if ( input.render == 1 ) [ self flushBuffer ];
		}
	}


	- ( void ) showKeyboard
	{
		if ( [ NSThread currentThread ] != [ NSThread mainThread ] )
		{
			[ self performSelectorOnMainThread : @selector( showKeyboard ) withObject : nil waitUntilDone : NO ];
		}
		else
		{
			if ( !self.isFirstResponder ) [self becomeFirstResponder];
		}
	}


	- ( void ) hideKeyboard
	{
		if ( [ NSThread currentThread ] != [ NSThread mainThread ] )
		{
			[ self performSelectorOnMainThread : @selector(hideKeyboard) withObject : nil waitUntilDone : NO ];
		}
		else
		{
			if ( self.isFirstResponder ) [self resignFirstResponder];
		}
	}


	- ( void ) keyboardWillShow : ( NSNotification* ) notification
	{
		NSDictionary* userInfo = [notification userInfo];
		CGSize keyboardSize = [[userInfo objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;

        input_t input = { 0 };
        input.type = kInputTypeVisibleFrameChanged;
        input.floata = ( float ) self.frame.size.width * [ [ UIScreen mainScreen ] scale ];
        input.floatb = ( float ) self.frame.size.height * [ [ UIScreen mainScreen ] scale ] - keyboardSize.height * [ [ UIScreen mainScreen ] scale ];

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
	}


	- ( void ) keyboardWillHide : ( NSNotification* ) notification
	{
        input_t input = { 0 };
        input.type = kInputTypeVisibleFrameChanged;
        input.floata = ( float ) self.frame.size.width * [ [ UIScreen mainScreen ] scale ];
        input.floatb = ( float ) self.frame.size.height * [ [ UIScreen mainScreen ] scale ];

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
	}
	

	- ( void ) insertText : ( NSString* ) text
	{
		char* chars = ( char* ) [ text cStringUsingEncoding : NSUTF8StringEncoding ];

        input_t input = { 0 };
        input.type = kInputTypeKeyPress;
        input.stringa = chars;
		
		if		( chars[0] == 127 ) input.key = kInputKeyTypeBackspace;
		else if ( chars[0] == 13  ) input.key = kInputKeyTypeReturn;
		else if ( chars[0] == 10  ) input.key = kInputKeyTypeReturn;
		else input.key = kInputKeyTypeNormal;

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
	}


	- ( void ) deleteBackward
    {
        input_t input = { 0 };
        input.type = kInputTypeKeyPress;
		input.key = kInputKeyTypeBackspace;

        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
    }

    
	- ( BOOL ) canBecomeFirstResponder
    {
        return YES;
    }


	- ( BOOL ) hasText
    {
        return YES;
    }


	- ( void ) drawView
    {
        [ context presentRenderbuffer : GL_RENDERBUFFER_OES ];
    }


	- ( void ) timerEvent
    {
        BOOL dorender = [staticMediaHandler timerEvent:controller];
        
        input_t input = { 0 };
        input.type = kInputTypeTimer;
        input.render = dorender ? 1 : 0;
        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
    }


	- ( void ) flushBuffer
    {
        [ context presentRenderbuffer : GL_RENDERBUFFER_OES ];
    }


	- ( void ) enterForeground : ( NSString* ) channel
    {
        input_t input = { 0 };
        input.type = kInputTypeEnterForeground;
        if ( channel != nil ) input.stringa = (char*) [channel cStringUsingEncoding:NSUTF8StringEncoding];
        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
    }


	- ( void ) enterBackground
    {
        input_t input = { 0 };
        input.type = kInputTypeEnterBackground;
        controller_input( controller , &input );
        if ( input.render == 1 ) [ self flushBuffer ];
    }
    

    - ( void ) copycb : ( char* ) string
    {
        UIPasteboard*  myPasteboard  = [ UIPasteboard generalPasteboard ];
        [ myPasteboard setString : [ NSString stringWithUTF8String : string ] ];
    }


    - ( void ) pastecb
    {
        UIPasteboard*  myPasteboard  = [UIPasteboard generalPasteboard];
        NSString* myString = [myPasteboard string];
        char* chars = ( char* ) [ myString cStringUsingEncoding : NSUTF8StringEncoding ];
        input_t input = { 0 };
        input.type = kInputTypePaste;
        input.stringa = chars;
        controller_input( controller , &input );
    }


    - ( void ) setVC:(UIViewController* )vcontroller
    {
        vvc = vcontroller;
    }


    - ( void ) showad : ( input_t* ) input
    {
        #ifdef ADMOB
        char* ad_appid = input->stringa;
        char* ad_unitid = input->stringb;
        [ staticAdHandler showAd : ad_unitid app : ad_appid controller : vvc ];
        #endif
    }

    - ( void ) rateapp
    {
        NSURL *url = [NSURL URLWithString:@"https://itunes.apple.com/us/app/mass-brawl/id1128196227?ls=1&mt=8"];
        if ([[UIApplication sharedApplication] canOpenURL:url]) {
           [[UIApplication sharedApplication] openURL:url options:[NSDictionary dictionary ] completionHandler:NULL ];
        }
    }

    - ( void ) openurl : ( char* ) string
    {
        NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:string]];
        if ([[UIApplication sharedApplication] canOpenURL:url]) {
           [[UIApplication sharedApplication] openURL:url options:[NSDictionary dictionary ] completionHandler:NULL ];
        }
    }

	@end


    	
	void controller_output( input_t* input )
	{
		if      ( strcmp( input->name , "showkeyboard" ) == 0 ) [ staticView showKeyboard ];
		else if ( strcmp( input->name , "hidekeyboard" ) == 0 )	[ staticView hideKeyboard ];
		else if ( strcmp( input->name , "copy" ) == 0 ) [ staticView copycb : input->stringa ];
		else if ( strcmp( input->name , "paste" ) == 0 ) [ staticView pastecb ];
		else if ( strcmp( input->name , "showad" ) == 0 ) [ staticView showad : input ];
		else if ( strcmp( input->name , "rateapp" ) == 0 ) [ staticView rateapp ];
		else if ( strcmp( input->name , "openurl" ) == 0 ) [ staticView openurl: input->stringa ];

        /* media related */
        
		else if ( strcmp( input->name , "loadmedia"         ) == 0 ) [ staticMediaHandler loadMedia : staticController input : input ];
		else if ( strcmp( input->name , "playmedia"         ) == 0 ) [ staticMediaHandler playMedia : staticController input : input ];
		else if ( strcmp( input->name , "stopmedia"         ) == 0 ) [ staticMediaHandler stopMedia : staticController input : input ];
		//else if ( strcmp( input->name , "selectmedia"       ) == 0 ) [ staticMediaHandler selectMedia: staticController input : input window : [staticView window] ];
		else if ( strcmp( input->name , "uploadmedia"       ) == 0 ) [ staticMediaHandler uploadMedia: staticController input : input ];
	}

