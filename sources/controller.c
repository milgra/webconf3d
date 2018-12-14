/*
  controller.c

  Created by Milan Toth on 26/09/15.
  Copyright (c) 2015 Milan Toth. All rights reserved.
*/

	#include "controller.h"
	
	void controller_setup_ui( controller_t* controller );
	void controller_resize( controller_t* controller , float width , float height );
	void controller_addavatar( controller_t* controller , GLint texture );
	void controller_removeavatar( controller_t* controller );
	void controller_play( controller_t* controller );

	char* controller_resourcepath( controller_t* controller , char* resourcename )
	{
		return mtcstr_fromformat( "%s/%s" , controller->path_resources , resourcename , NULL );
	}

	/* alloc */

	controller_t* controller_alloc( environment_t env )
	{
		assert( env.path_resources != NULL );
		assert( env.path_library != NULL );

		char* fontpath = mtcstr_fromformat( "%s/Avenir.ttc" , env.path_resources , NULL );
		
		controller_t* result = mtmem_calloc( sizeof(controller_t ) , controller_dealloc );

		result->ticks = 0;
		result->scale = env.scale;
		result->width = env.width;
		result->height = env.height;
		result->windowed = env.windowed;
		result->path_library = env.path_library;
		result->path_resources = env.path_resources;

		result->zoom = 1.0;
		result->trans = 0.0;
		result->angle = 0.0;
		result->uiwidth = 1800;
		result->uiheight = 1200;

		result->font = font_alloc( fontpath );
		result->tilemap = tilemap_alloc( 4096, 4096 );
		result->blackmap = mtbmp_alloc( 1024 , 1024 );
		result->screenmap = tilemap_alloc( 4096, 4096 );
		
		result->labels = mtvec_alloc( );
		result->avatars = mtvec_alloc( );
		result->elements = mtvec_alloc( );
		
		result->renderer = renderer3D_alloc( env.width , env.height , env.scale );
		result->animator = trafoanimator_alloc( );

		ogl_texture_create_linear( &result->textures[0] , 4096 , 4096 );	// tilemap
		ogl_texture_create_linear( &result->textures[1] , 4096 , 4096 );	// screen
		
		result->videos[0] = "video1.mp4";
		result->videos[1] = "video2.mp4";
		result->videos[2] = "video3.mp4";
		result->videos[3] = "video4.mp4";
		result->videos[4] = "video5.mp4";
		result->videos[5] = "video6.mp4";
		result->videos[6] = "video7.mp4";
		
		for ( int index = 0 ; index < 7 ; index++ )
		{		
			ogl_texture_create_linear( &result->textures[2 + index ] , 1024 , 1024 );
			controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( result , result->videos[ index ] ) , .inta = result->textures[ 2 + index ] , .floata = 0.0 }) );
		}
		
		mtbmp_fill_with_color( result->blackmap , 0, 0, 1024, 1024, 0x000000FF );
		controller_resize( result , env.width , env.height );
		controller_setup_ui( result );
		
		result->playhead = 1;
		controller_play( result );

		return result;
	}

	/* dealloc */

	void controller_dealloc( void* pointer )
	{
        controller_t* controller = pointer;
		
		mtmem_release( controller->path_library );
		mtmem_release( controller->path_resources );
	}
	
	/* resize event */
	
	void controller_resize( controller_t* controller , float width , float height )
	{
		controller->width = width;
		controller->height = height;

		float camera_fov_y = M_PI / 4.0;
		float camera_eye_z = ( height / 2.0 ) / ( tanf( camera_fov_y / 2.0 ) );
		float camera_ratio = width / controller->uiwidth;

		matrix4_t view_matrix;
		view_matrix = matrix4_defaulttranslation( 0.0 , 0.0 , -camera_eye_z );
		view_matrix = matrix4_multiply( view_matrix , matrix4_defaultscale( camera_ratio , camera_ratio , camera_ratio ) );
		controller->pers_matrix = matrix4_defaultperspective( camera_fov_y , width / height , 50.0 , -1000.0 );
		controller->proj_matrix = matrix4_multiply( controller->pers_matrix , view_matrix );

		controller->animator->trans_actual.x = -controller->uiwidth / 2.0;		
		controller->animator->trans_actual.y = controller->uiheight / 2.0;		
		controller->animator->trans_actual.z = 0.0;
		controller->animator->scale_actual.x = 1.0;
		controller->animator->scale_actual.y = 1.0;
		controller->animator->scale_actual.z = 1.0;
		
		renderer3D_resize( controller->renderer , controller->width , controller->height );
	}
	
	/* upload whole scene */
	
	void controller_uploadscene( controller_t* controller )
	{
		renderer3D_reset( controller->renderer );
		for ( int index = 0 ; index < controller->elements->length ; index++ )
		{
			element3D_t* element = controller->elements->data[ index ];
			renderer3D_addelement( controller->renderer , element->quad );
		}	
		for ( int index = 0 ; index < controller->labels->length ; index++ )
		{
			element3D_t* element = controller->labels->data[ index ];
			renderer3D_addelement( controller->renderer , element->quad );
		}	
	}
	
	/* create room walls */
	
	void controller_create_walls( controller_t* controller )
	{
		vector2_t* floorcoords = tilemap_get( controller->tilemap, "floor1.png" );

		element3D_t* wall_a = element3D_alloc( "walla" , 2000.0 , -5000.0 , floorcoords , controller->textures[0]  );
		element3D_t* wall_b = element3D_alloc( "wallb" , 5000.0 , -2000.0 , floorcoords , controller->textures[0]  );
		element3D_t* wall_c = element3D_alloc( "wallc" , 5000.0 , -2000.0 , floorcoords , controller->textures[0]  );
		element3D_t* wall_d = element3D_alloc( "walld" , 2000.0 , -2000.0 , floorcoords , controller->textures[0]  );

		element3D_rotate( wall_a , M_PI , 0.0 , 0.0 );
		element3D_translate( wall_a , -100.0 , -900.0 , -500.0 );
		
		element3D_rotate( wall_b , 0.0 , -M_PI / 2 , 0.0 );
		element3D_rotate( wall_b , -M_PI/2 , 0.0 , 0.0 );
		element3D_translate( wall_b , -100.0 , -900.0 , -500.0 );

		element3D_rotate( wall_c , 0.0 , -M_PI / 2 , 0.0 );
		element3D_rotate( wall_c , -M_PI/2 , 0.0 , 0.0 );
		element3D_translate( wall_c , 1900.0 , -900.0 , -500.0 );	

		element3D_rotate( wall_d , -M_PI / 2 , 0.0 , 0.0 );
		element3D_translate( wall_d , -100.0 ,-900.0,-500.0 );

		mtvec_adddata( controller->elements , wall_a );
		mtvec_adddata( controller->elements , wall_b );
		mtvec_adddata( controller->elements , wall_c );
		mtvec_adddata( controller->elements , wall_d );
	}
	
	/* create screen and screen avatar */

	void controller_create_screen( controller_t* controller )
	{
		vector2_t* floorcoords = tilemap_get( controller->tilemap, "floor1.png" );

		element3D_t* screen = element3D_alloc( "screen" , 1400.0 , -800.0 , floorcoords , controller->textures[1]  );
		element3D_t* avatar_g = element3D_alloc( "avatar_presenter" , 320 , -180.0 , floorcoords , controller->textures[0] );

		element3D_rotate( screen , -M_PI / 4 , 0.0 , 0.0 );
		element3D_translate( screen , 200.0 ,-600.0,-500.0 );

		element3D_rotate( avatar_g , -M_PI / 4 , 0.0 , 0.0 );
		element3D_translate( avatar_g , 1600.0 ,-600.0,-500.0 );

		mtvec_adddata( controller->elements , screen );
		mtvec_adddata( controller->elements , avatar_g );
		
		controller->screen = screen;
		controller->presenter = avatar_g;		

        mtbmp_t* screenmap = image_bmp_from_png( controller_resourcepath( controller , "screen1.png" )	);
        vector2_t* coords = tilemap_put( controller->screenmap, "screen.png" , screenmap , 4 );

        controller->screen->quad->texcoords = coords;
	}

	/* create screen and screen avatar */

	element3D_t* controller_create_label( controller_t* controller, float size , mtstr_t* text , uint32_t colorb , uint32_t colorf )
	{
        textstyle_t style =
        {
            .align = 1,
            .editable = 0,
            .selectable = 0,
            .multiline = 0,
            .autosize = 0,
            .uppercase = 0,
            
            .textsize = size,
            .marginsize = 0.0,
            .cursorsize = 0.0,
            
            .textcolor = colorf,
            .backcolor = colorb
        };
        
        
        mtbmp_t* labelmap = font_render_text( 200.0 , size * 2 , text , controller->font , style , NULL , NULL );
		char* name = mtstr_bytes( text );
		vector2_t* labelcoords = tilemap_put( controller->tilemap, name , labelmap , 0 );
		element3D_t* label = element3D_alloc( name , labelmap->width , -labelmap->height , labelcoords , controller->textures[0] );

		return label;
	}
	
	/* setup ui */
	
	void controller_setup_ui( controller_t* controller )
	{
		/* prepare tiles */

		mtbmp_t* floormap = image_bmp_from_png( controller_resourcepath( controller , "floor1.png" ) );
		mtbmp_t* screenmap = image_bmp_from_png( controller_resourcepath( controller , "floor1.png" ) );

		tilemap_put( controller->tilemap, "floor1.png" , floormap , 0 );
		tilemap_put( controller->screenmap, "floor1.png" , screenmap , 0 );

		mtmem_release( floormap );
		mtmem_release( screenmap );

		controller_create_walls( controller );
		controller_create_screen( controller );

		element3D_t* label = controller_create_label( controller, 120.0 , mtstr_frombytes( "Waiting for the presenter..." ) , 0x00000000 , 0xFFFFFFFF );
		element3D_translate( label , 900.0 - label->quad->width / 2.0  ,-300.0,-500.0 );
		mtvec_adddata( controller->elements , label );

		controller->labelA = label;

		label = controller_create_label( controller, 40.0 , mtstr_frombytes( "User the arrows to navigate through the demo." ) , 0x00000000 , 0xFFFFFFFF );
		element3D_translate( label , 900.0 - label->quad->width / 2.0  ,-400.0,-500.0 );
		mtvec_adddata( controller->elements , label );
		
		controller->labelB = label;
		
		controller_uploadscene( controller );
	}


	void controller_play( controller_t* controller )
	{
		if ( controller->playhead < 0 ) controller->playhead = 0;
		if ( controller->playhead > 7 ) controller->playhead = 7;
		switch ( controller->playhead )
		{
			case 0 :
			{
				while ( controller->avatars->length > 0 ) controller_removeavatar( controller );
				break;
			}
			case 1 :
			{
				if ( controller->labelA != NULL )
				{
					mtvec_removedata( controller->elements , controller->labelA );
					controller->labelA = NULL;
				}
				if ( controller->labelB != NULL )
				{
					mtvec_removedata( controller->elements , controller->labelB );
					controller->labelB = NULL;
				}

				while ( controller->avatars->length > 0 ) controller_removeavatar( controller );
				controller_addavatar( controller , controller->textures[2] );
				ogl_texture_update_rgba( controller->textures[ 2 ] , 1024, 1024, controller->blackmap->bytes );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "video1.mp4" ) , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );
				break;
			}
			case 2 :
			{
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "video1.mp4" ) , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "video2.mp4" ) , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );

				mtbmp_t* screenmap = image_bmp_from_png( controller_resourcepath( controller , "screen1.png" )	);

				tilemap_reset( controller->screenmap );
				vector2_t* coords = tilemap_put( controller->screenmap, "screen.png" , screenmap , 4 );

				controller->screen->quad->texcoords = coords;

				vector2_t* fullcoords = mtmem_calloc( sizeof( vector2_t ) * 4 , NULL );
				fullcoords[0] = vector2_init( 0.0 , 0.0 );
				fullcoords[1] = vector2_init( 1.0 , 0.0 );
				fullcoords[2] = vector2_init( 0.0 , 1.0 );
				fullcoords[3] = vector2_init( 1.0 , 1.0 );
				#ifdef ASMJS
				fullcoords[0] = vector2_init( 0.0 , 1.0 );
				fullcoords[1] = vector2_init( 1.0 , 1.0 );
				fullcoords[2] = vector2_init( 0.0 , 0.0 );
				fullcoords[3] = vector2_init( 1.0 , 0.0 );
				#endif

				controller->presenter->quad->texture = controller->textures[ 3 ];
				controller->presenter->quad->texcoords = fullcoords;

				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "nothing2.mp4" ) , .chara = 1 , .inta = controller->textures[ 3 ] , .floata = 1.0 , .intb = 1 }) );

				controller_uploadscene( controller );
				break;
			}
			case 3 :
			{
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "video2.mp4" ) , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );

				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "nothing1.mp4" ) , .chara = 1 , .inta = controller->textures[ 2 ] , .floata = 1.0 , .intb= 1}) );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "nothing2.mp4" ) , .chara = 1 , .inta = controller->textures[ 3 ] , .floata = 1.0 , .intb= 1 }) );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "nothing3.mp4" ) , .chara = 1 , .inta = controller->textures[ 4 ] , .floata = 1.0 , .intb= 1 }) );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "video3.mp4" ) , .chara = 0 , .inta = controller->textures[ 5 ] , .floata = 1.0 , .intb= 1}) );
				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "video4.mp4" ) , .chara = 0 , .inta = controller->textures[ 6 ] , .floata = 1.0 , .intb= 1}) );

				ogl_texture_update_rgba( controller->textures[ 2 ] , 1024, 1024, controller->blackmap->bytes );
				ogl_texture_update_rgba( controller->textures[ 3 ] , 1024, 1024, controller->blackmap->bytes );
				ogl_texture_update_rgba( controller->textures[ 4 ] , 1024, 1024, controller->blackmap->bytes );
				ogl_texture_update_rgba( controller->textures[ 5 ] , 1024, 1024, controller->blackmap->bytes );
				ogl_texture_update_rgba( controller->textures[ 6 ] , 1024, 1024, controller->blackmap->bytes );

				for ( int index = 0 ; index < 4 ; index++ )
				{
					controller_addavatar( controller , controller->textures[2] );
					controller_addavatar( controller , controller->textures[3] );
					controller_addavatar( controller , controller->textures[4] );
					controller_addavatar( controller , controller->textures[5] );
					controller_addavatar( controller , controller->textures[6] );
				}
				break;
			}
			case 4 :
			{
				while ( controller->avatars->length > 1 ) controller_removeavatar( controller );
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "nothing1.mp4" ) , .chara = 1 , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "nothing2.mp4" ) , .chara = 1 , .inta = controller->textures[ 3 ] , .floata = 1.0 }) );
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "nothing3.mp4" ) , .chara = 1 , .inta = controller->textures[ 4 ] , .floata = 1.0 }) );
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "video3.mp4" ) , .chara = 0 , .inta = controller->textures[ 5 ] , .floata = 1.0 }) );
				controller_output( &(( input_t) { .name = "stopvideo", .stringa = controller_resourcepath( controller , "video4.mp4" ) , .chara = 0 , .inta = controller->textures[ 6 ] , .floata = 1.0 }) );

				controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "video5.mp4" ) , .inta = controller->textures[ 2 ] , .floata = 1.0 }) );
				break;
			}
			case 6 :
			{
				mtbmp_t* screenmap = image_bmp_from_png( controller_resourcepath( controller , "screen2.png" )	);

				tilemap_reset( controller->screenmap );
				vector2_t* coords = tilemap_put( controller->screenmap, "screen.png" , screenmap , 4 );

				controller->screen->quad->texcoords = coords;

				controller_uploadscene( controller );
				break;
			}
			case 7 :
			{
				mtbmp_t* screenmap = image_bmp_from_png( controller_resourcepath( controller , "screen3.png" )	);

				tilemap_reset( controller->screenmap );
				vector2_t* coords = tilemap_put( controller->screenmap, "screen.png" , screenmap , 4 );

				controller->screen->quad->texcoords = coords;

				
				controller_uploadscene( controller );
				break;
			}
//			case 8 :
//			{
//				while ( controller->avatars->length > 0 ) controller_removeavatar( controller );
//
//				element3D_t* label = controller_create_label( controller, 120.0 , mtstr_frombytes( "milgra.com/controlroom" ) , 0x00000000 , 0x000000FF );
//				element3D_translate( label , 900.0 - label->quad->width / 2.0  ,-300.0,-500.0 );
//				mtvec_adddata( controller->elements , label );
//				controller_uploadscene( controller );
//
//				break;
//			}
		}
	}
	
	/* next step */

	void controller_next( controller_t* controller )
	{
		controller->playhead += 1;
		controller_play( controller );
	}

	/* prev step */

	void controller_prev( controller_t* controller )
	{
		controller->playhead -= 1;
		controller_play( controller );	
	}

	void controller_up( controller_t* controller )
	{
		controller_addavatar(controller,controller->textures[2]);	
	}

	void controller_down( controller_t* controller )
	{
		controller_removeavatar(controller);				
	}
	
	/* render event */

	void controller_render( controller_t* controller , char force )
	{
		if ( trafoanimator_update( controller->animator ) == 0 && controller->selected == NULL )
		{
			controller->animator->angle_actual.y = controller->angle;
			controller->animator->trans_actual.y = controller->uiheight / 2.0 + controller->trans;		
		}

		/* animate to element */

		matrix4_t trans_matrix = matrix4_defaulttranslation( controller->animator->trans_actual.x , controller->animator->trans_actual.y , controller->animator->trans_actual.z );
		matrix4_t angle_matrix = matrix4_defaultrotation( controller->animator->angle_actual.x , controller->animator->angle_actual.y , controller->animator->angle_actual.z );
		matrix4_t scale_matrix = matrix4_defaultscale( controller->animator->scale_actual.x , controller->animator->scale_actual.y , controller->animator->scale_actual.z );
		matrix4_t zoom_matrix = matrix4_defaultscale( controller->zoom , controller->zoom , controller->zoom );
		matrix4_t pan_matrix = matrix4_defaulttranslation( controller->panvector.x * controller->zoom , controller->panvector.y * controller->zoom , 0.0 );
		
		matrix4_t final = matrix4_defaultidentity();
		
		final = matrix4_multiply( trans_matrix , final );
		final = matrix4_multiply( angle_matrix , final );
		final = matrix4_multiply( scale_matrix , final );

		if ( controller->selected != NULL && controller->zoom > 1.0 )
		{
			final = matrix4_multiply( pan_matrix , final );
			final = matrix4_multiply( zoom_matrix , final );
		}

		final = matrix4_multiply( controller->proj_matrix , final );
		
		renderer3D_updateprojectionmatrix( controller->renderer , final );
		
		controller->pers_matrix = final;

		if ( controller->tilemap->updated == 1 )
		{
			ogl_texture_update_rgba( controller->textures[0], 4096 , 4096 , controller->tilemap->bitmap->bytes );
			controller->tilemap->updated = 0;
		}

		if ( controller->screenmap->updated == 1 )
		{
			ogl_texture_update_rgba( controller->textures[1], 4096 , 4096 , controller->screenmap->bitmap->bytes );
			controller->screenmap->updated = 0;
		}
		
		renderer3D_draw( controller->renderer );

		controller_output( &((input_t) { .name = "flushbuffer" }) );
	}
	
	/* position avatars */
	
	void controller_updateavatars( controller_t* controller )
	{
		float width = 1920;
		float height = -1080;
		
		float newhth = height;
		float newwth = width;
		
		float placewth = 2220;
		float placehth = -750;
		
		float x = 0.0;
		float y = 0.0;
		
		int row = 0;
		int col = 0;
		int rows = 0;
		int cols = 0;
		
		/* calculate fitting col and row count */

		while ( 1 )
		{
			cols = floorf(placewth / newwth);
			rows = floorf(placehth / newhth);
			if ( cols * rows >= controller->avatars->length ) break;
			newwth -= 10.0;
			newhth -= 10.0 * ( height / width );
		}
		
		if ( ( cols - 1 ) * rows >= controller->avatars->length ) cols -= 1;
		
		/* starting positions */
		
		x = controller->uiwidth / 2.0 - placewth / 2.0  + ( placewth - cols * newwth ) / 2.0;
		y = controller->uiheight / 2.0 - 400.0 + ( placehth - rows * newhth ) / 2.0;
		
		for ( int index = 0 ; index < controller->avatars->length ; index++ )
		{
			element3D_t* avatar = controller->avatars->data[ index ];
			element3D_t* label = controller->labels->data[ index ];
			
			element3D_resettransformations( avatar );
			element3D_resettransformations( label );
			
			avatar->quad->width = newwth - 5.0;
			avatar->quad->height = newhth + 5.0;
			
			float dist = placewth / 2.0 - ( col * newwth + newwth / 2.0 );
			float ratio = dist / ( placewth / 2.0);

			if ( controller->avatars->length == 1 ) ratio = 0.0;
			
			/* rotatio around y axis a little to look towards the user */

			element3D_rotate_central( avatar , 0.0 , M_PI / 9 * ratio , 0.0 );						
			element3D_translate( avatar , x + col * newwth, y + row * newhth ,-600.0 );

			float dy = 1000.0 - cosf( M_PI / 9 * ratio ) * 1000.0;
			
			element3D_translate_central( avatar , 0.0 , 0.0 , dy * 3.2 );
			
			/* move label over avatar */
			
			label->quad->matrix = avatar->quad->matrix;
			float lratio = label->quad->width / label->quad->height;
			label->quad->height = avatar->quad->height / 8.0;
			label->quad->width = label->quad->height * lratio;
			element3D_translate( label , 5.0 , -5.0 , 0.0 );
			
			col += 1;
			if ( col == cols )
			{
				col = 0;
				row += 1;
			}
		}
	}
	
	/* add new avatar */

	void controller_addavatar( controller_t* controller , GLint texture )
	{
        printf( "ADDAVATAR %i\n" , texture );
    
        char name[50];
        snprintf( name , 50 , "master_%i" , controller->avatars->length );
		
		vector2_t* fullcoords = mtmem_calloc( sizeof( vector2_t ) * 4 , NULL );
		fullcoords[0] = vector2_init( 0.0 , 0.0 );
		fullcoords[1] = vector2_init( 1.0 , 0.0 );
		fullcoords[2] = vector2_init( 0.0 , 1.0 );
		fullcoords[3] = vector2_init( 1.0 , 1.0 );

		element3D_t* avatar = element3D_alloc( name , 1920 , -1080 , fullcoords , texture );
		element3D_t* label = controller_create_label( controller, 60.0 , mtstr_frombytes( name ) , 0xFFFFFF00 , 0xFFFFFFFF );

		mtvec_adddata( controller->elements , avatar );
		mtvec_adddata( controller->avatars , avatar );
		mtvec_adddata( controller->labels , label );
				
		controller_updateavatars( controller );
		controller_uploadscene( controller );
	}
	
	/* remove avatar */

	void controller_removeavatar( controller_t* controller )
	{
		if ( controller->avatars->length > 0 )
		{
			int index = controller->avatars->length - 1;
			element3D_t* avatar = controller->avatars->data[index];
			element3D_t* label = controller->labels->data[index];
			
			mtvec_removedata( controller->elements , avatar );
			mtvec_removedata( controller->avatars , avatar );
			mtvec_removedata( controller->labels , label );
			
			controller_updateavatars( controller );
			controller_uploadscene( controller );
		}
	}
	
	/* check element under press point */
	
	element3D_t* controller_check_overlay( controller_t* controller , float x , float y )
	{
		/* create screen-plane-normal vectors */

		vector4_t front = vector4_init( x , controller->height - y , -1.0 , 0.0 );
		vector4_t back = vector4_init( x , controller->height - y , 0.0 , 0.0 );

		/* unproject vectors to 3d world */
		
		vector3_t frontprj = matrix4_screen_to_world_coords( controller->pers_matrix , front , controller->width , controller->height );
		vector3_t backprj = matrix4_screen_to_world_coords( controller->pers_matrix , back , controller->width , controller->height );
		
		/* check for intersection */
		
		for ( int index = 0 ; index < controller->elements->length ; index++ )
		{
			element3D_t* element = controller->elements->data[ index ];
			if ( strstr( element->name , "wall" ) == NULL )
			{
				vector3_t is = quad3D_intersect( element->quad , frontprj , backprj );
				if ( is.x != FLT_MAX ) return element;
			}
		}
		return NULL;
	}

	/* selects element */
	
	void controller_touch_down( controller_t* controller , input_t* input )
	{	
		element3D_t* selected = controller_check_overlay( controller , input->floata , input->floatb ); 

		if ( selected != NULL )
		{
			if ( controller->selected == selected )
			{
				controller->selected = NULL;
				controller->zoom = 1.0;
				controller->animator->trans_final = vector3_init( -controller->uiwidth / 2.0 , controller->uiheight / 2.0 + controller->trans , 0.0 );
				controller->animator->scale_final = vector3_init( 1.0 , 1.0 , 1.0 );
				controller->animator->angle_final = vector3_init( 0.0 , controller->angle , 0.0 );
				controller->animator->counter = 0;
			}
			else
			{
				controller->selected = selected;
									
				float wratio = 1800 / selected->quad->width;
				float hratio = -1600 / selected->quad->height;
				float ratio = wratio < hratio ? wratio : hratio;

				vector3_t center = quad3D_center( selected->quad );
				
				controller->animator->trans_final = vector3_init( -center.x , -center.y , -center.z );
				controller->animator->angle_final = vector3_init( -selected->angle.x  , -selected->angle.y , -selected->angle.z );
				controller->animator->scale_final = vector3_init( ratio , ratio , ratio );
				controller->animator->counter = 0;
			}
		}
	}
	
	/* mouse moved, pand and turn when not selected, zoom and pan when selected */

	void controller_mouse_move( controller_t* controller , input_t* input )
	{
		if (input->floata > 0 && input->floata < controller->width )
		{
			controller->panvector.x = controller->width / 2.0 - input->floata;
			controller->panvector.y = -controller->height / 2.0 - input->floatb;
		
			controller->angle = -( controller->width / 2.0 - input->floata ) / ( controller->width / 2.0 );

            printf( "angle %f\n" , controller->angle );
			
			if ( input->floatb > -controller->height / 1.5 )
			{
				controller->trans = ( -controller->height / 1.5 + input->floatb ) * 0.8;
			}
			else controller->trans = 0.0;
		}	
	}

	/* input event from the os */

	void controller_input( controller_t* controller , input_t* input )
	{
		switch ( input->type )
		{
			case kInputTypeTimer : 
				if ( controller->animator->counter < 40 ) controller_render( controller , 0 );
				break;
				
			case kInputTypeRender : 
				break;

			case kInputTypeResize : 
				controller_resize( controller , input->floata , input->floatb ); 
				break;
				
			case kInputTypeTouchDown : 
				controller_touch_down( controller , input ); 
				break;
				
			case kInputTypeTouchDrag : 
				controller_mouse_move( controller , input );
				break;
			
			case kInputTypeScroll :
			
				controller->zoom -= input->floatb / 1000.0;
				break;
				
			case kInputTypeKeyPress :
			
				if ( input->stringa[0] == 'f' ) controller_output( &(( input_t) { .name = "maximizewindow" }) );
				if ( input->stringa[0] == 'p' ) 
				{
					controller_output( &(( input_t) { .name = "playvideo", .stringa = controller_resourcepath( controller , "nothing1.mp4" ) , .chara = 1 , .inta = controller->textures[2] , .floata = 1.0 }) );
				}
				if ( input->key == kInputKeyTypeRightArrow ) controller_next( controller );
				if ( input->key == kInputKeyTypeLeftArrow ) controller_prev( controller );
				if ( input->key == kInputKeyTypeUpArrow ) controller_up( controller );
				if ( input->key == kInputKeyTypeDownArrow ) controller_down( controller );
				
				break;
		}

		controller_render( controller , 1 );
        
        input->render = 1;
	}
