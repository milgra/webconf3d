//
//  GLView.h
//  minimal
//
//  Created by Milan Toth on 2017. 01. 06..
//  Copyright © 2017. milgra. All rights reserved.
//

    #import <Cocoa/Cocoa.h>

    @interface GLView : NSOpenGLView

	- ( id ) initWithFrame : ( NSRect ) frameRect delegate : ( id ) delegate;
	- ( void ) load;
	- ( void ) render;
    - ( void ) pushNotificationTokenReceived : ( NSString* ) token;

    @end
