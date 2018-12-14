//
//  MediaHandler.h
//  minimal
//
//  Created by Milan Toth on 2017. 01. 06..
//  Copyright © 2017. milgra. All rights reserved.
//

    #import <Cocoa/Cocoa.h>
	#import <Accelerate/Accelerate.h>
    #import <Foundation/Foundation.h>
	#import <AVFoundation/AVFoundation.h>
    #import "controller.h"

    @interface MediaHandler : NSObject

	- ( void ) loadMedia : ( controller_t* ) controller input : ( input_t* ) input;
	- ( void ) playMedia : ( controller_t* ) controller input : ( input_t* ) input;
	- ( void ) stopMedia : ( controller_t* ) controller input : ( input_t* ) input;
	- ( void ) selectMedia :  ( controller_t* ) controller input : ( input_t* ) input window : ( NSWindow* ) window;
    - ( void ) uploadMedia : ( controller_t* ) controller input : ( input_t* ) input;
	- ( BOOL ) timerEvent : ( controller_t* ) controller;

    @end
