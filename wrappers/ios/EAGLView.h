//
//  EAGLView.h
//  chat
//
//  Created by Milan Toth on 2017. 01. 07..
//  Copyright © 2017. Milan Toth. All rights reserved.
//

    #import <Foundation/Foundation.h>
	#import <UIKit/UIKit.h>


    @interface EAGLView : UIView <UIKeyInput>
    {

    }

	- ( void ) enterForeground : ( NSString* ) channel;
    - ( void ) enterBackground;
    - ( void ) flushBuffer;
    - ( void ) load;
    - ( void ) pushNotificationTokenReceived : ( NSString* ) token;
    - ( void ) setVC:(UIViewController* )vcontroller;

    @end
