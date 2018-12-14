//
//  MediaHandler.m
//  minimal
//
//  Created by Milan Toth on 2017. 01. 06..
//  Copyright © 2017. milgra. All rights reserved.
//

    #import "MediaHandler.h"
    #import "settings.h"

    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>

    static NSMutableDictionary* audioPlayers;
    static NSMutableDictionary* videoPlayers;

    static NSString* tempLocalPath;
    static NSString* tempLocalMime;

    BOOL dorender = NO;

    @implementation MediaHandler

    - ( id ) init
    {
        self = [super init];
        
		audioPlayers = [[NSMutableDictionary alloc]init];
		videoPlayers = [[NSMutableDictionary alloc]init];
        
        return self;
    }


    - (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSString *,id> *)change
                       context:(void *)context
    {
        if ([keyPath isEqualToString:@"status"])
        {
            AVPlayerItemStatus status = AVPlayerItemStatusUnknown;
            // Get the status change from the change dictionary
            NSNumber *statusNumber = change[NSKeyValueChangeNewKey];
            if ([statusNumber isKindOfClass:[NSNumber class]]) {
                status = statusNumber.integerValue;
            }
            // Switch over the status
            switch (status) {
                case AVPlayerItemStatusReadyToPlay:
                {
                    AVPlayer* player = ( AVPlayer* ) context;
                    // uncomment when streaming from HTTP
                    // [player play];
                    // Ready to Play
                    break;
                }
                case AVPlayerItemStatusFailed:
                    // Failed. Examine AVPlayerItem.error
                    break;
                case AVPlayerItemStatusUnknown:
                    // Not ready
                    break;
            }
        }
    }

    - (NSURLRequest *)connection: (NSURLConnection *)connection
                 willSendRequest: (NSURLRequest *)request
                redirectResponse: (NSURLResponse *)redirectResponse;
    {
        if ( redirectResponse != NULL )
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            ^{
                NSString* paths = [[redirectResponse URL] absoluteString];
                NSString* newpaths = [[request URL] absoluteString];
                NSMutableDictionary* group = videoPlayers[ paths ];

                if ( group == nil ) return; // shouldnt happen
                
                if ( [group objectForKey:@"inited"] == nil )
                {
                    NSLog( @"playing video from %@", newpaths );
                    // NSURL* url = [NSURL fileURLWithPath:paths];
                    NSURL* url = [NSURL URLWithString:newpaths];
                    AVAsset* asset = [AVAsset assetWithURL:url];
                    NSDictionary* settings = @{ (id)kCVPixelBufferPixelFormatTypeKey : [NSNumber numberWithInt:kCVPixelFormatType_32BGRA] };
                
                    AVPlayerItemVideoOutput* videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:settings];
                    videoOutput.suppressesPlayerRendering = YES;
                    
                    AVPlayerItem* videoPlayerItem = [AVPlayerItem playerItemWithAsset:asset];
                    [videoPlayerItem addOutput:videoOutput];
					
                    NSKeyValueObservingOptions options =
                        NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew;
                    
                    AVPlayer* player = [AVPlayer playerWithPlayerItem:videoPlayerItem];

                    player.actionAtItemEnd = AVPlayerActionAtItemEndNone;
					
                    // Register as an observer of the player item's status property
                    [videoPlayerItem addObserver:self
                                 forKeyPath:@"status"
                                    options:options
                                    context:player];
                    
                    [group setObject:player forKey:@"player"];
                    [group setObject:paths forKey:@"path"];
                    [group setObject:videoPlayerItem forKey:@"playeritem"];
                    [group setObject:[NSNumber numberWithInt:0] forKey:@"inited"];
                    
                    videoPlayers[ paths ] = group;
                }
                
                AVPlayer* player = [group objectForKey:@"player"];
                //[player seekToTime:kCMTimeZero];
                [player play];

            });
            return nil;
        }
        return request;
    }

	- ( void ) loadRemoteMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        char* path = input->stringa;
        //char* name = input->stringb;
        char* mime = input->stringc;
        
        mtstr_t* username = settings_getstring( input->map , "username" );
        mtstr_t* password = settings_getstring( input->map , "password" );
        mtstr_t* clientid = settings_getstring( input->map , "clientid" );

        char* usernames = mtstr_bytes( username );
        char* passwords = mtstr_bytes( password );
        char* clientids = mtstr_bytes( clientid );
        
        int texture = input->inta;

        NSString* paths = [ NSString stringWithFormat : @"%s" , path ];
        NSString* mimes = [ NSString stringWithFormat : @"%s" , mime ];

        NSString* userName = [ NSString stringWithFormat : @"%s" , usernames ];
        NSString* passWord = [ NSString stringWithFormat : @"%s" , passwords ];
        NSString* clientID = [ NSString stringWithFormat : @"%s" , clientids ];
        
        if ( [mimes containsString:@"image"] )
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            ^{
                NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];

                [request setURL:[NSURL URLWithString:paths]];
                [request setHTTPMethod:@"GET"];
                [request setCachePolicy: NSURLRequestReloadIgnoringCacheData ];

                [request setValue: userName forHTTPHeaderField: @"c36-username"];
                [request setValue: passWord forHTTPHeaderField: @"c36-password"];
                [request setValue: clientID forHTTPHeaderField: @"c36-clientid"];
                
                // make the connection to the web
                NSURLResponse* response = [[NSURLResponse alloc] init];
                NSError* error = nil;
                NSData* imageData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
                if ( error != nil ) NSLog(@"Image load error %@" , error );
                
                dispatch_sync(dispatch_get_main_queue(),
                ^{
                    NSImage* image = [[NSImage alloc] initWithData:imageData];
                    NSBitmapImageRep *bitmap = [ [ NSBitmapImageRep alloc ] initWithData : [ image TIFFRepresentation ] ];
                    
                    glBindTexture( GL_TEXTURE_2D , texture );
                    glTexImage2D( GL_TEXTURE_2D , 0 , GL_RGBA , (GLint)[bitmap pixelsWide] , (GLint)[bitmap pixelsHigh] , 0 , GL_RGB , GL_UNSIGNED_BYTE , [ bitmap bitmapData ] );

                    input_t input = { 0 };
                    input.type = kInputTypeMediaLoaded;
                    input.inta = texture;
                    input.floata = (float)[bitmap pixelsWide];
                    input.floatb = (float)[bitmap pixelsHigh];
                    input.stringa = (char*) [paths cStringUsingEncoding:NSUTF8StringEncoding];
                    
                    controller_input( controller , &input );
                    
                    dorender = 1;

                    [bitmap release];
                    [image release];
                 });
            });
        }
        else if ( [mimes containsString:@"video"] )
        {
            /* redirect version */
            
            NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];

            [request setURL:[NSURL URLWithString:paths]];
            [request setHTTPMethod:@"GET"];
            [request setCachePolicy: NSURLRequestReloadIgnoringCacheData ];

            [request setValue: userName forHTTPHeaderField: @"c36-username"];
            [request setValue: passWord forHTTPHeaderField: @"c36-password"];
            [request setValue: clientID forHTTPHeaderField: @"c36-clientid"];
            
            NSMutableDictionary* group = [[ NSMutableDictionary alloc] init ];
            
            [group setObject:paths forKey:@"path"];
            [group setObject:[NSNumber numberWithInt:texture] forKey:@"texture"];
            videoPlayers[ paths ] = group;

            [ NSURLConnection connectionWithRequest : request delegate : self ];
            
            [ request release ];
        }
    }

	- ( void ) selectMedia :  ( controller_t* ) controller input : ( input_t* ) input window : ( NSWindow* ) window
    {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setFloatingPanel:YES];
        [panel setCanChooseDirectories:NO];
        [panel setCanChooseFiles:YES];
        [panel setAllowsMultipleSelection:YES];
        [panel retain];
       
        [panel beginSheetModalForWindow:window completionHandler :^(NSInteger result)
        {
            if (result == NSFileHandlingPanelOKButton)
            {
                NSArray* urls = [panel URLs];
                NSURL* url = [urls objectAtIndex:0];
                
                NSString *extension = [url pathExtension];
                NSString *exportedUTI = (NSString*)UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (CFStringRef)extension, NULL);
                NSString *mimeType = (NSString*)UTTypeCopyPreferredTagWithClass((CFStringRef)exportedUTI, kUTTagClassMIMEType);
                
                tempLocalPath = [[url absoluteString] retain];
                tempLocalMime = [mimeType retain ];

                input_t input = { 0 };
                input.type = kInputTypeMediaSelected;
                input.stringa = (char*) [[url absoluteString] cStringUsingEncoding: NSUTF8StringEncoding ];
                input.stringb = (char*) [mimeType cStringUsingEncoding: NSUTF8StringEncoding ];
                controller_input( controller , &input );
            }
            [panel release];
        }];
    }

    - ( void ) uploadMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        NSString *remoteString = [NSString stringWithUTF8String: input->stringa ];
        NSString *localString = tempLocalPath;
        NSString *mimeString = tempLocalMime;
        
        NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString: localString ] ];

        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
        ^{
            NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];

            [ request setURL : [ NSURL URLWithString:remoteString ] ];
            [ request setHTTPMethod : @"PUT" ];
            [ request setHTTPBody : data ];
            [ request setValue: mimeString forHTTPHeaderField:@"Content-Type" ];

            // make the connection to the web
            NSHTTPURLResponse* response = [[NSHTTPURLResponse alloc] init];
            [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:nil];
            
            NSLog(@"RESOONSE %@" , response);
            
            if ( [response statusCode ] == 200 )
            {
                dispatch_sync(dispatch_get_main_queue(),
                ^{
                    input_t input = { 0 };
                    input.type = kInputTypeMediaUploaded;
                    input.stringa = (char*) [[localString lastPathComponent] cStringUsingEncoding: NSUTF8StringEncoding ];
                    input.stringb = (char*) [[[response URL] lastPathComponent] cStringUsingEncoding:NSUTF8StringEncoding];
                    input.stringc = (char*) [mimeString cStringUsingEncoding:NSUTF8StringEncoding];
                    controller_input( controller , &input );
                 });
            }
            
            //[request release ];
            //[response release ];
        });
        
        [tempLocalPath release ];
        [tempLocalMime release ];
    }

    - ( void ) loadLocalMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        NSString* paths = [ NSString stringWithFormat : @"%s" , input->stringa ];
        NSString *extension = [ [ NSString stringWithUTF8String:input->stringa ] pathExtension ];
        NSString *exportedUTI = (NSString*)UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (CFStringRef)extension, NULL);
        NSString *mimeType = (NSString*)UTTypeCopyPreferredTagWithClass((CFStringRef)exportedUTI, kUTTagClassMIMEType);
    
        if ( [ mimeType containsString : @"image" ] )
        {
            
        }
        else if ( [ mimeType containsString : @"audio" ] )
        {
            AVAudioPlayer* player = audioPlayers[ paths ];
            
            if ( player == nil )
            {
                NSURL* fileURL = [[NSURL alloc] initFileURLWithPath:paths];
                NSError* error;
                player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:&error];
                if ( error != nil ) NSLog( @"ERROR %@" , error );
                audioPlayers[ paths ] = player;
                [fileURL release ];
            }
        }
        else if ( [ mimeType containsString : @"video" ] )
        {
            int texture = input->inta;
            NSMutableDictionary* group = [[ NSMutableDictionary alloc] init ];
            
            [ group setObject : paths forKey : @"path" ];
            [ group setObject : [ NSNumber numberWithInt : texture ] forKey : @"texture" ];

            NSURL* url = [ NSURL fileURLWithPath : paths ];
            AVAsset* asset = [ AVAsset assetWithURL : url ];
            NSDictionary* settings = @{ (id)kCVPixelBufferPixelFormatTypeKey : [NSNumber numberWithInt:kCVPixelFormatType_32BGRA] };

            AVPlayerItemVideoOutput* videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:settings];
            videoOutput.suppressesPlayerRendering = YES;

            AVPlayerItem* videoPlayerItem = [AVPlayerItem playerItemWithAsset:asset];
            [videoPlayerItem addOutput:videoOutput];
            
            NSKeyValueObservingOptions options = NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew;
            
            AVPlayer* player = [AVPlayer playerWithPlayerItem:videoPlayerItem];
			dispatch_queue_t mainQueue = dispatch_get_main_queue();
            [player addPeriodicTimeObserverForInterval:
				CMTimeMakeWithSeconds(0.5, NSEC_PER_SEC)
				queue:mainQueue
				usingBlock:^(CMTime time)
				{
					AVPlayerItem* current = player.currentItem;
					
					CMTime cdur = current.duration;
					CMTime ccur = current.currentTime;
					
					if ( !CMTIME_IS_INDEFINITE(cdur) && !CMTIME_IS_INDEFINITE(ccur) )
					{
					int64_t dur = current.duration.value / current.duration.timescale;
					int64_t cur = current.currentTime.value / current.currentTime.timescale;
                   if (cur > dur - 1 )
                   {
                   		if ( player.actionAtItemEnd==AVPlayerActionAtItemEndPause )
                   		{
                   		[player seekToTime:CMTimeMake(0, 1)];
                   		}
                   }
                   }
				}];
			
            // Register as an observer of the player item's status property
            [videoPlayerItem addObserver:self
                         forKeyPath:@"status"
                            options:options
                            context:player];
            
            [group setObject:player forKey:@"player"];
            [group setObject:paths forKey:@"path"];
            [group setObject:videoPlayerItem forKey:@"playeritem"];
            [group setObject:[NSNumber numberWithInt:0] forKey:@"inited"];
			
			videoPlayers[ paths ] = group;
			
        }
    }

	- ( void ) loadMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        if ( strstr( input->stringa , "http://" ) != NULL ) [ self loadRemoteMedia : controller  input : input ];
        else [ self loadLocalMedia : controller input : input ];
    }

    /* play media or set volume */

	- ( void ) playMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        NSString* paths = [ NSString stringWithFormat : @"%s" , input->stringa ];
        NSString *extension = [ [ NSString stringWithUTF8String:input->stringa ] pathExtension ];
        NSString *exportedUTI = (NSString*)UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (CFStringRef)extension, NULL);
        NSString *mimeType = (NSString*)UTTypeCopyPreferredTagWithClass((CFStringRef)exportedUTI, kUTTagClassMIMEType);
    
        if ( [ mimeType containsString : @"image" ] )
        {

        }
        else if ( [ mimeType containsString : @"audio" ] )
        {
            AVAudioPlayer* player = audioPlayers[ paths ];
			
            if ( player == nil )
            {
            	[self loadMedia:controller input:input];
            	player = audioPlayers[ paths ];
			}

            if ( ![player isPlaying] ) [player play];
            player.volume = input->floata;
            player.numberOfLoops = input->inta == 1 ? -1 : 0;
        }
        else if ( [ mimeType containsString : @"video" ] )
        {
            NSMutableDictionary* group = videoPlayers[ paths ];

            AVPlayer* player = [group objectForKey:@"player"];

            if ( player == nil )
            {
            	[self loadMedia:controller input:input];
            	group = videoPlayers[ paths ];
            	player = [group objectForKey:@"player"];
			}
			
			if ( input->intb == 1 ) player.actionAtItemEnd = AVPlayerActionAtItemEndPause;
			else player.actionAtItemEnd = AVPlayerActionAtItemEndNone;

            int texture = input->inta;
			
            [ group setObject : [ NSNumber numberWithInt : texture ] forKey : @"texture" ];

			[player seekToTime: CMTimeMake(0, 1)];
            [player play ];
            player.volume = input->floata;
        }
    }

    /* stop media */

	- ( void ) stopMedia : ( controller_t* ) controller input : ( input_t* ) input
    {
        NSString* paths = [ NSString stringWithFormat : @"%s" , input->stringa ];
        NSString *extension = [ [ NSString stringWithUTF8String:input->stringa ] pathExtension ];
        NSString *exportedUTI = (NSString*)UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (CFStringRef)extension, NULL);
        NSString *mimeType = (NSString*)UTTypeCopyPreferredTagWithClass((CFStringRef)exportedUTI, kUTTagClassMIMEType);
    
        if ( [ mimeType containsString : @"image" ] )
        {
            
        }
        else if ( [ mimeType containsString : @"audio" ] )
        {
            AVAudioPlayer* player = audioPlayers[ paths ];
            [player stop];
        }
        else if ( [ mimeType containsString : @"video" ] )
        {
            NSMutableDictionary* group = videoPlayers[ paths ];
            AVPlayer* player = [group objectForKey:@"player"];
            [player pause];
        }
    }

    /* render video frames to texture */

	- ( BOOL ) timerEvent : ( controller_t* ) controller
    {
        [videoPlayers enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL* stop) {
            NSMutableDictionary* group = value;
            AVPlayer* player = [group objectForKey:@"player"];
            
            if ( [ player rate ] > 0 )
            {
                CMTime itemTime = [[player currentItem] currentTime];
				
                AVPlayerItem* item = [player currentItem];
                AVPlayerItemVideoOutput* output = ( AVPlayerItemVideoOutput* ) [[item outputs] objectAtIndex:0];

                if ( [ output hasNewPixelBufferForItemTime : itemTime ] )
                {
                    dorender = YES;
                
                    CVPixelBufferRef buffer = [ output copyPixelBufferForItemTime : itemTime itemTimeForDisplay : nil ];
                    size_t width = CVPixelBufferGetWidth(buffer);
                    size_t height = CVPixelBufferGetHeight(buffer);

                    CVPixelBufferLockBaseAddress(buffer, 0);

                    unsigned char converteddata[ width * height * 4 ];
                    
                    vImage_Buffer video_vib = { CVPixelBufferGetBaseAddress(buffer), height, width, width * 4 };
                    vImage_Buffer converted_video_vib = { converteddata, height, width, width * 4 };

                    uint8_t permuteMap[4] = { 2, 1, 0, 3 }; // BGRA -> RGBA
                    vImagePermuteChannels_ARGB8888(&video_vib, &converted_video_vib, permuteMap, 0);

                    NSNumber* texturenumber = [group objectForKey:@"texture"];
                    
                    glBindTexture( GL_TEXTURE_2D , [texturenumber intValue] );
                    /* !!! use subimage after the first upload */
                    //glTexSubImage2D( GL_TEXTURE_2D , 0 , 0 , 0 , (GLsizei)width , (GLsizei)height , GL_RGBA , GL_UNSIGNED_BYTE , converteddata );
                    glTexImage2D( GL_TEXTURE_2D , 0 , GL_RGBA , (GLsizei)width , (GLsizei)height , 0 , GL_RGBA , GL_UNSIGNED_BYTE , converteddata );
                    
                    CVPixelBufferUnlockBaseAddress(buffer, 0);
                    CVBufferRelease(buffer);
                    
                    NSNumber* inited = [group objectForKey:@"inited"];
                    if ( [inited intValue] == 0 )
                    {
                        [group setObject : [NSNumber numberWithInt:1] forKey: @"inited" ];
                        NSString* path = [group objectForKey:@"path"];

                        input_t input = { 0 };
                        input.type = kInputTypeMediaLoaded;
                        input.inta = [texturenumber intValue];
                        input.floata = (float)width;
                        input.floatb = (float)height;
                        input.stringa = (char*) [path cStringUsingEncoding:NSUTF8StringEncoding];
                        controller_input( controller , &input );
                    }
                }
            }
            else [player seekToTime:CMTimeMake(0, 1)];

        }];
        
        BOOL result = dorender;
        dorender = NO;
        return result;
    }

    @end
