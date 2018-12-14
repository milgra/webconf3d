//
//  AppDelegate.m
//  c36
//
//  Created by Milan Toth on 22/01/15.
//  Copyright (c) 2015 Milan Toth. All rights reserved.
//

    #import "AppDelegate.h"
    #import "EAGLView.h"


    static NSString* requestedchannel = NULL;

	@interface AppDelegate ( )
	{
		EAGLView* view;
		UIWindow* window;
		UIViewController* vcontroller;
	}

	@end


	@implementation AppDelegate


	- ( BOOL ) application : ( UIApplication* ) application didFinishLaunchingWithOptions : ( NSDictionary* ) launchOptions 
	{
		view = [ [ EAGLView alloc ] initWithFrame : [ [ UIScreen mainScreen ] bounds ] ];
		window 	= [ [ UIWindow alloc ] initWithFrame : [ [ UIScreen mainScreen ] bounds ] ];
		vcontroller = [ [ UIViewController alloc ] init ];
		
		[ vcontroller setView : view ];
		[ window setRootViewController : vcontroller ];
		[ window makeKeyAndVisible ];
        [ view setVC: vcontroller ];
		[ view load ];
        
        /* enable instant touch over status bar */
        
        for ( UIGestureRecognizer* recognizer in window.gestureRecognizers )
        {
            [recognizer setDelaysTouchesBegan:NO];
            [recognizer setDelaysTouchesEnded:NO];
        }

		#ifdef PUSH

//        NSDictionary *notification = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
//        if ( notification ) [ self application : application didReceiveRemoteNotification : notification fetchCompletionHandler : ^(UIBackgroundFetchResult result) { } ];
//        
//        
//        UIUserNotificationType types = ( UIUserNotificationType ) ( UIUserNotificationTypeBadge |
//                                                                    UIUserNotificationTypeSound |
//                                                                    UIUserNotificationTypeAlert );
//        
//        UIUserNotificationSettings *mySettings =
//        [UIUserNotificationSettings settingsForTypes:types categories:nil];
//        
//        [ [ UIApplication sharedApplication ] registerUserNotificationSettings : mySettings ];
//        [ [ UIApplication sharedApplication ] registerForRemoteNotifications ];

        #endif
        
		return YES;
	}


    - ( void ) application : ( UIApplication* ) application didReceiveRemoteNotification : ( NSDictionary* ) userInfo fetchCompletionHandler : ( void (^)( UIBackgroundFetchResult ) ) completionHandler
    {
        if (userInfo[@"aps"] && userInfo[@"aps"][@"badge"]) {
            [[UIApplication sharedApplication] setApplicationIconBadgeNumber: [userInfo[@"aps"][@"badge"] unsignedLongValue]];
        }
        
        if ( application.applicationState == UIApplicationStateInactive || application.applicationState == UIApplicationStateBackground  )
        {
            requestedchannel = [ userInfo[ @"channel" ] retain];
        }
    }

    #ifdef PUSH

//    - ( void ) application : ( UIApplication* ) app didRegisterForRemoteNotificationsWithDeviceToken :  ( NSData*) devToken
//    {
//        NSMutableString *hexToken = [[NSMutableString alloc] init];
//        const unsigned char *buf = [devToken bytes];
//        NSInteger i;
//        for (i=0; i<[devToken length]; ++i) {
//            [hexToken appendFormat:@"%02lX", (unsigned long)buf[i]];
//        }
//
//        [ view pushNotificationTokenReceived:hexToken ];
//        NSLog ( @"Device registered to push notifications with token: %@" , hexToken );
//    }
//
//
//    - ( void )application : ( UIApplication* ) app didFailToRegisterForRemoteNotificationsWithError : ( NSError* )err
//    {
//        NSLog( @"Error in registration. Error: %@" , err );
//    }

    #endif


	- ( void ) applicationWillResignActive : ( UIApplication* ) application
    {
        [ view enterBackground ];
    }


	- ( void ) applicationDidBecomeActive : ( UIApplication* ) application
    {
        [ view enterForeground : requestedchannel ];
        if ( requestedchannel != nil ) [requestedchannel release];
        requestedchannel = nil;
    }


	@end
	
