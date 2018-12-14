//
//  AdHandler.m
//  chat
//
//  Created by Milan Toth on 2017. 01. 07..
//  Copyright © 2017. Milan Toth. All rights reserved.
//

	#import <GoogleMobileAds/GoogleMobileAds.h>
	#import <CoreLocation/CoreLocation.h>
    #import "AdHandler.h"

    GADInterstitial* interstitial;
    UIViewController* vcontroller;

    @implementation AdHandler

	/* show admob ad */
	
	- ( void ) showAd : ( char* ) adUnitId app : ( char* ) appId controller : ( UIViewController* ) vcontroller
	{
        if ( interstitial == nil )
        {
            [GADMobileAds configureWithApplicationID : [ NSString stringWithUTF8String: appId ] ];

            interstitial = [ [ GADInterstitial alloc ] initWithAdUnitID : [ NSString stringWithUTF8String: adUnitId ] ];
            
            vcontroller = vcontroller;

            GADRequest *request = [GADRequest request];
            // Request test ads on devices you specify. Your test device ID is printed to the console when
            // an ad request is made.
            request.testDevices = @[ kGADSimulatorID , @"2a3f63ff863ed9f2970e82eb3b6bc16d" , @"6df473e2bced27257abf21f77c80ccf7" ];

            CLLocationManager* locationManager = [ [ CLLocationManager alloc ] init ];
            CLLocation *currentLocation = locationManager.location;
            if ( currentLocation )
            {
                [ request 
                    setLocationWithLatitude : currentLocation.coordinate.latitude
                    longitude : currentLocation.coordinate.longitude
                    accuracy : currentLocation.horizontalAccuracy];
            }
          
            [interstitial loadRequest:request];
        }
        else
        {
            [interstitial presentFromRootViewController:vcontroller];
            [interstitial release];
            interstitial = nil;
        }
	}

    /// Called when an interstitial ad request succeeded.
    - (void)interstitialDidReceiveAd:(GADInterstitial *)ad {
        NSLog(@"interstitialDidReceiveAd");
        if (interstitial.isReady) {
            [interstitial presentFromRootViewController:vcontroller];
        }
    }

    /// Called when an interstitial ad request failed.
    - (void)interstitial:(GADInterstitial *)ad
        didFailToReceiveAdWithError:(GADRequestError *)error {
      NSLog(@"interstitial:didFailToReceiveAdWithError: %@", [error localizedDescription]);
    }

    /// Called just before presenting an interstitial.
    - (void)interstitialWillPresentScreen:(GADInterstitial *)ad {
      NSLog(@"interstitialWillPresentScreen");
    }

    /// Called before the interstitial is to be animated off the screen.
    - (void)interstitialWillDismissScreen:(GADInterstitial *)ad {
      NSLog(@"interstitialWillDismissScreen");
    }

    /// Called just after dismissing an interstitial and it has animated off the screen.
    - (void)interstitialDidDismissScreen:(GADInterstitial *)ad {
      NSLog(@"interstitialDidDismissScreen");
      [interstitial release];
      interstitial = nil;
    }

    /// Called just before the app will background or terminate because the user clicked on an
    /// ad that will launch another app (such as the App Store).
    - (void)interstitialWillLeaveApplication:(GADInterstitial *)ad {
      NSLog(@"interstitialWillLeaveApplication");
    }

    @end
