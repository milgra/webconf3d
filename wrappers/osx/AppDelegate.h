//
//  AppDelegate.h
//  c36
//
//  Created by Milan Toth on 22/01/15.
//  Copyright (c) 2015 Milan Toth. All rights reserved.
//

    #import <Cocoa/Cocoa.h>
	
	@interface AppDelegate : NSObject < NSApplicationDelegate , NSWindowDelegate >

	- ( void ) maximizeWindow;
	- ( void ) closeWindow;

	@end

	@interface GLWindow : NSWindow

	@end
