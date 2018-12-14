//
//  AdHandler.h
//  chat
//
//  Created by Milan Toth on 2017. 01. 07..
//  Copyright © 2017. Milan Toth. All rights reserved.
//

    #import <Foundation/Foundation.h>

    @interface AdHandler : NSObject

	- ( void ) showAd : ( char* ) adUnitId app : ( char* ) appId controller : ( UIViewController* ) vcontroller;

    @end
