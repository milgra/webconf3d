//
//  AppDelegate.m
//  c36
//
//  Created by Milan Toth on 22/01/15.
//  Copyright (c) 2015 Milan Toth. All rights reserved.
//

	#import "AppDelegate.h"
    #import "GLView.h"

	#define wth 800
	#define hth 600


	@implementation GLWindow
	
	- ( BOOL ) canBecomeKeyWindow {	return YES; }
	- ( BOOL ) canBecomeMainWindow { return YES; }
	
	@end


	@implementation AppDelegate
	{
		BOOL fullscreen;
		NSRect lastframe;
        GLView* view;
		NSWindow* window;
	}


	- ( void ) createWindow
	{
		NSRect screenRect = [ [ NSScreen mainScreen ] frame ];
		NSRect windowRect = NSMakeRect( ( screenRect.size.width - wth ) / 2 , ( screenRect.size.height  - hth ) / 2 , wth ,	hth );
		
        window = [ [ GLWindow alloc ] 
			initWithContentRect : windowRect
			styleMask           : NSWindowStyleMaskBorderless | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
			backing             : NSBackingStoreBuffered
			defer               : YES ];
									  
        [ window setLevel : NSNormalWindowLevel ];
        [ window setDelegate : self ];
        [ window setHasShadow : YES ];
        [ window setContentView : view ];
        [ window setAcceptsMouseMovedEvents : YES ];
		[ window setMinSize: NSMakeSize( 200, 300 ) ];
			
        [ window makeKeyAndOrderFront : self ];
        [ window makeFirstResponder : view ];
        [ window makeMainWindow ];
	}
	

    - ( void ) applicationDidFinishLaunching : ( NSNotification* ) theNotification
    {
        view = [ [ GLView alloc ] initWithFrame : NSMakeRect( 0 , 0 , wth , hth ) delegate : self ];

		[ self createWindow ];
		[ view load ];
        
		[ NSApp activateIgnoringOtherApps : YES ];
        
        #ifdef PUSH
        [ NSApp registerForRemoteNotificationTypes : NSRemoteNotificationTypeAlert |
                                                     NSRemoteNotificationTypeBadge |
                                                     NSRemoteNotificationTypeSound ];
        #endif
    }


	- ( BOOL ) applicationShouldHandleReopen : ( NSApplication* ) sender hasVisibleWindows : ( BOOL ) flag
	{
		if ( window == nil ) [ self createWindow ];
		[view render];
		return NO;
	}


    - ( void ) application : ( NSApplication* ) application didRegisterForRemoteNotificationsWithDeviceToken : ( NSData* ) devToken
    {
        NSMutableString *hexToken = [ [ NSMutableString alloc ] init ];
        const unsigned char *buf = [ devToken bytes ];
        
        NSInteger i;
        for ( i = 0 ; i < [ devToken length ]; ++i ) {
            [ hexToken appendFormat : @"%02lX", (unsigned long)buf[ i ] ];
        }
        
        [ view pushNotificationTokenReceived : hexToken ];

        [ hexToken release ];
    }


    - ( void ) application : ( NSApplication* ) application didFailToRegisterForRemoteNotificationsWithError : ( NSError* ) error
    {
        //NSLog(@"*** Application can't register %@", [error localizedDescription]);
    }


    - ( void ) application : ( NSApplication* ) application didReceiveRemoteNotification : ( NSDictionary<NSString*, id> * ) userInfo
    {
        //NSLog(@"*** Got a remote notification: %@", [userInfo description]);
    }


	- ( void ) maximizeWindow
	{
        if (view.inFullScreenMode)
        {
            [view exitFullScreenModeWithOptions:nil];
            [ window makeKeyAndOrderFront : self ];
            [ window makeFirstResponder : view ];
            [ window makeMainWindow ];
        }
        else
        {
            NSApplicationPresentationOptions options = NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar;
            [view enterFullScreenMode:[NSScreen mainScreen] withOptions:@{ NSFullScreenModeApplicationPresentationOptions : @(options) }];
        }
    }


	- ( void ) closeWindow
    {
        [ window close ];
    }


	- ( void ) windowWillClose : ( NSNotification* ) notification
    {
        window = nil;
    }


	@end
