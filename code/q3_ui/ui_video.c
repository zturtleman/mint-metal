/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
#include "ui_local.h"

void GraphicsOptions_MenuInit( void );

/*
=======================================================================

DRIVER INFORMATION MENU

=======================================================================
*/


#define DRIVERINFO_FRAMEL	"menu/art/frame2_l"
#define DRIVERINFO_FRAMER	"menu/art/frame1_r"
#define DRIVERINFO_BACK0	"menu/art/back_0"
#define DRIVERINFO_BACK1	"menu/art/back_1"

static char* driverinfo_artlist[] = 
{
	DRIVERINFO_FRAMEL,
	DRIVERINFO_FRAMER,
	DRIVERINFO_BACK0,
	DRIVERINFO_BACK1,
	NULL,
};

#define ID_DRIVERINFOBACK	100

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menubitmap_s	back;
	menubitmap_s	framel;
	menubitmap_s	framer;
	char			stringbuff[1024];
	char*			strings[64];
	int				numstrings;
} driverinfo_t;

static driverinfo_t	s_driverinfo;

/*
=================
DriverInfo_Event
=================
*/
static void DriverInfo_Event( void* ptr, int event )
{
	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_DRIVERINFOBACK:
			UI_PopMenu();
			break;
	}
}

/*
=================
DriverInfo_MenuDraw
=================
*/
static void DriverInfo_MenuDraw( void )
{
	int	i;
	int	y;

	Menu_Draw( &s_driverinfo.menu );

	UI_DrawString( 320, 80, "VENDOR", UI_CENTER|UI_SMALLFONT, text_small_title_color );
	UI_DrawString( 320, 152, "PIXELFORMAT", UI_CENTER|UI_SMALLFONT, text_small_title_color );
	UI_DrawString( 320, 192, "EXTENSIONS", UI_CENTER|UI_SMALLFONT, text_small_title_color );

	UI_DrawString( 320, 80+16, cgs.glconfig.vendor_string, UI_CENTER|UI_SMALLFONT, text_color_normal );
	UI_DrawString( 320, 96+16, cgs.glconfig.version_string, UI_CENTER|UI_SMALLFONT, text_color_normal );
	UI_DrawString( 320, 112+16, cgs.glconfig.renderer_string, UI_CENTER|UI_SMALLFONT, text_color_normal );
	UI_DrawString( 320, 152+16, va ("color(%d-bits) Z(%d-bits) stencil(%d-bits)", cgs.glconfig.colorBits, cgs.glconfig.depthBits, cgs.glconfig.stencilBits), UI_CENTER|UI_SMALLFONT, text_color_normal );

	// double column
	y = 192+16;
	for (i=0; i<s_driverinfo.numstrings/2; i++) {
		UI_DrawString( 320-4, y, s_driverinfo.strings[i*2], UI_RIGHT|UI_SMALLFONT, text_color_normal );
		UI_DrawString( 320+4, y, s_driverinfo.strings[i*2+1], UI_LEFT|UI_SMALLFONT, text_color_normal );
		y += SMALLCHAR_HEIGHT;
	}

	if (s_driverinfo.numstrings & 1)
		UI_DrawString( 320, y, s_driverinfo.strings[s_driverinfo.numstrings-1], UI_CENTER|UI_SMALLFONT, text_color_normal );
}

/*
=================
DriverInfo_Cache
=================
*/
void DriverInfo_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!driverinfo_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(driverinfo_artlist[i]);
	}
}

/*
=================
UI_DriverInfo_Menu
=================
*/
static void UI_DriverInfo_Menu( void )
{
	char*	eptr;
	int		i;
	int		len;

	// zero set all our globals
	memset( &s_driverinfo, 0 ,sizeof(driverinfo_t) );

	DriverInfo_Cache();

	s_driverinfo.menu.fullscreen = qtrue;
	s_driverinfo.menu.draw       = DriverInfo_MenuDraw;

	s_driverinfo.banner.generic.type  = MTYPE_BTEXT;
	s_driverinfo.banner.generic.x	  = 320;
	s_driverinfo.banner.generic.y	  = 16;
	s_driverinfo.banner.string		  = "DRIVER INFO";
	s_driverinfo.banner.color	      = text_banner_color;
	s_driverinfo.banner.style	      = UI_CENTER;

	s_driverinfo.framel.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framel.generic.name  = DRIVERINFO_FRAMEL;
	s_driverinfo.framel.generic.flags = QMF_INACTIVE;
	s_driverinfo.framel.generic.x	  = 0;
	s_driverinfo.framel.generic.y	  = 78;
	s_driverinfo.framel.width  	      = 256;
	s_driverinfo.framel.height  	  = 329;

	s_driverinfo.framer.generic.type  = MTYPE_BITMAP;
	s_driverinfo.framer.generic.name  = DRIVERINFO_FRAMER;
	s_driverinfo.framer.generic.flags = QMF_INACTIVE;
	s_driverinfo.framer.generic.x	  = 376;
	s_driverinfo.framer.generic.y	  = 76;
	s_driverinfo.framer.width  	      = 256;
	s_driverinfo.framer.height  	  = 334;

	s_driverinfo.back.generic.type	   = MTYPE_BITMAP;
	s_driverinfo.back.generic.name     = DRIVERINFO_BACK0;
	s_driverinfo.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_driverinfo.back.generic.callback = DriverInfo_Event;
	s_driverinfo.back.generic.id	   = ID_DRIVERINFOBACK;
	s_driverinfo.back.generic.x		   = 0;
	s_driverinfo.back.generic.y		   = 480-64;
	s_driverinfo.back.width  		   = 128;
	s_driverinfo.back.height  		   = 64;
	s_driverinfo.back.focuspic         = DRIVERINFO_BACK1;

  // TTimo: overflow with particularly long GL extensions (such as the gf3)
  // https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=399
  // NOTE: could have pushed the size of stringbuff, but the list is already out of the screen
  // (no matter what your resolution)
  Q_strncpyz(s_driverinfo.stringbuff, cgs.glconfig.extensions_string, 1024);

	// build null terminated extension strings
	eptr = s_driverinfo.stringbuff;
	while ( s_driverinfo.numstrings<40 && *eptr )
	{
		while ( *eptr && *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ')
			s_driverinfo.strings[s_driverinfo.numstrings++] = eptr;

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	// safety length strings for display
	for (i=0; i<s_driverinfo.numstrings; i++) {
		len = strlen(s_driverinfo.strings[i]);
		if (len > 32) {
			s_driverinfo.strings[i][len-1] = '>';
			s_driverinfo.strings[i][len]   = '\0';
		}
	}

	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.banner );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framel );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.framer );
	Menu_AddItem( &s_driverinfo.menu, &s_driverinfo.back );

	UI_PushMenu( &s_driverinfo.menu );
}

/*
=======================================================================

GRAPHICS OPTIONS MENU

=======================================================================
*/

#define GRAPHICSOPTIONS_FRAMEL	"menu/art/frame2_l"
#define GRAPHICSOPTIONS_FRAMER	"menu/art/frame1_r"
#define GRAPHICSOPTIONS_BACK0	"menu/art/back_0"
#define GRAPHICSOPTIONS_BACK1	"menu/art/back_1"
#define GRAPHICSOPTIONS_ACCEPT0	"menu/art/accept_0"
#define GRAPHICSOPTIONS_ACCEPT1	"menu/art/accept_1"

#define ID_BACK2		101
#define ID_FULLSCREEN	102
#define ID_LIST			103
#define ID_MODE			104
#define ID_DRIVERINFO	105
#define ID_GRAPHICS		106
#define ID_DISPLAY		107
#define ID_SOUND		108
#define ID_NETWORK		109
#define ID_RATIO		110
#define ID_FPS			111

typedef struct {
	menuframework_s	menu;

	menutext_s		banner;
	menubitmap_s	framel;
	menubitmap_s	framer;

	menutext_s		graphics;
	menutext_s		display;
	menutext_s		sound;
	menutext_s		network;

	menulist_s		list;
	menulist_s		ratio;
	menulist_s		mode;
	menulist_s		fps;
	menulist_s		driver;
	menulist_s		tq;
	menulist_s  	fs;
	menulist_s  	lighting;
	menulist_s  	flares;
	menulist_s  	texturebits;
	menulist_s  	colordepth;
	menulist_s  	geometry;
	menulist_s  	filter;
	menulist_s  	multisample;
	menutext_s		driverinfo;

	menubitmap_s	apply;
	menubitmap_s	back;
} graphicsoptions_t;

typedef struct
{
	int mode;
	qboolean fullscreen;
	int tq;
	int lighting;
	int colordepth;
	int texturebits;
	int geometry;
	int filter;
	int multisample;
	qboolean flares;
	int fps;
} InitialVideoOptions_s;

static InitialVideoOptions_s	s_ivo;
static graphicsoptions_t		s_graphicsoptions;	

static InitialVideoOptions_s s_ivo_templates[] =
{
	// very high
	{
		6, qtrue, 3, 0, 2, 2, 4, 5, 2, qtrue, 4	// Note: If r_availableModes is found, mode is changed to -2.
	},
	// high
	{
		6, qtrue, 3, 0, 2, 2, 3, 2, 1, qtrue, 4
	},
	// normal
	{
		6, qtrue, 3, 0, 0, 2, 2, 1, 0, qtrue, 3
	},
	// fast
	{
		4, qtrue, 2, 0, 1, 0, 1, 0, 0, qfalse, 3
	},
	// fastest
	{
		3, qtrue, 1, 1, 1, 0, 0, 0, 0, qfalse, 3
	},
	// custom
	{
		3, qtrue, 1, 0, 0, 0, 1, 0, 0, qfalse, 3
	}
};

#define NUM_IVO_TEMPLATES ( ARRAY_LEN( s_ivo_templates ) )

static const char *builtinResolutions[ ] =
{
	"320x240",
	"400x300",
	"512x384",
	"640x480",
	"800x600",
	"960x720",
	"1024x768",
	"1152x864",
	"1280x1024",
	"1600x1200",
	"2048x1536",
	"856x480",
	NULL
};

static const char *knownRatios[ ][2] =
{
	{ "1.25:1", "5:4"   },
	{ "1.33:1", "4:3"   },
	{ "1.50:1", "3:2"   },
	{ "1.56:1", "14:9"  },
	{ "1.60:1", "16:10" },
	{ "1.67:1", "5:3"   },
	{ "1.78:1", "16:9"  },
	{ NULL    , NULL    }
};

#define MAX_RESOLUTIONS	32

static const char* ratios[ MAX_RESOLUTIONS ];
static char ratioBuf[ MAX_RESOLUTIONS ][ 14 ];
static int ratioToRes[ MAX_RESOLUTIONS ];
static int resToRatio[ MAX_RESOLUTIONS ];

static char resbuf[ MAX_STRING_CHARS ];
static const char* detectedResolutions[ MAX_RESOLUTIONS ];
static char currentResolution[ 20 ];

static const char** resolutions = builtinResolutions;
static qboolean resolutionsDetected = qfalse;

/*
=================
GraphicsOptions_FindBuiltinResolution
=================
*/
static int GraphicsOptions_FindBuiltinResolution( int mode )
{
	int i;

	if( !resolutionsDetected )
		return mode;

	// Display resolution
	if( mode == 0 )
		return -2;

	if( mode < 0 )
		return -1;

	for( i = 0; builtinResolutions[ i ]; i++ )
	{
		if( !Q_stricmp( builtinResolutions[ i ], detectedResolutions[ mode ] ) )
			return i;
	}

	return -1;
}

/*
=================
GraphicsOptions_FindDetectedResolution
=================
*/
static int GraphicsOptions_FindDetectedResolution( int mode )
{
	int i;

	if( !resolutionsDetected )
		return mode;

	// Display resolution
	if( mode == -2 )
		return 0;

	if( mode < 0 )
		return -1;

	for( i = 0; detectedResolutions[ i ]; i++ )
	{
		if( !Q_stricmp( builtinResolutions[ mode ], detectedResolutions[ i ] ) )
			return i;
	}

	return -1;
}

/*
=================
GraphicsOptions_AspectString
=================
*/
const char *GraphicsOptions_AspectString( float w, float h ) {
	static char str[ sizeof(ratioBuf[0]) ];
	int i;

	Com_sprintf( str, sizeof(str), "%.2f:1", w / h );

	// rename common ratios ("1.33:1" -> "4:3")
	for( i = 0; knownRatios[i][0]; i++ ) {
		if( !Q_stricmp( str, knownRatios[i][0] ) ) {
			Q_strncpyz( str, knownRatios[i][1], sizeof( str ) );
			break;
		}
	}

	return str;
}

/*
=================
GraphicsOptions_GetAspectRatios
=================
*/
static void GraphicsOptions_GetAspectRatios( void )
{
	int i, r;

	// build ratio list from resolutions
	for( r = 0; resolutions[r]; r++ )
	{
		int w, h;
		char *x;
		char str[ sizeof(ratioBuf[0]) ];

		// calculate resolution's aspect ratio
		if (strchr(resolutions[r], '(')) {
			w = cgs.glconfig.displayWidth;
			h = cgs.glconfig.displayHeight;
			Com_sprintf( str, sizeof(str), "Auto (%s)", GraphicsOptions_AspectString( w, h ) );
		} else {
			x = strchr( resolutions[r], 'x' ) + 1;
			Q_strncpyz( str, resolutions[r], x-resolutions[r] );
			w = atoi( str );
			h = atoi( x );
			Q_strncpyz( str, GraphicsOptions_AspectString( w, h ), sizeof( str ) );
		}

		// add ratio to list if it is new
		// establish res/ratio relationship
		for( i = 0; ratioBuf[i][0]; i++ )
		{
			if( !Q_stricmp( str, ratioBuf[i] ) )
				break;
		}
		if( !ratioBuf[i][0] )
		{
			Q_strncpyz( ratioBuf[i], str, sizeof(ratioBuf[i]) );
			ratioToRes[i] = r;
		}

		ratios[r] = ratioBuf[r]; 
		resToRatio[r] = i; 
	}

	ratios[r] = NULL;
}

/*
=================
GraphicsOptions_GetInitialVideo
=================
*/
static void GraphicsOptions_GetInitialVideo( void )
{
	s_ivo.colordepth  = s_graphicsoptions.colordepth.curvalue;
	s_ivo.mode        = s_graphicsoptions.mode.curvalue;
	s_ivo.fullscreen  = s_graphicsoptions.fs.curvalue;
	s_ivo.fps         = s_graphicsoptions.fps.curvalue;
	s_ivo.tq          = s_graphicsoptions.tq.curvalue;
	s_ivo.lighting    = s_graphicsoptions.lighting.curvalue;
	s_ivo.flares      = s_graphicsoptions.flares.curvalue;
	s_ivo.geometry    = s_graphicsoptions.geometry.curvalue;
	s_ivo.filter      = s_graphicsoptions.filter.curvalue;
	s_ivo.multisample = s_graphicsoptions.multisample.curvalue;
	s_ivo.texturebits = s_graphicsoptions.texturebits.curvalue;

#if 0
	Com_Printf( "DEBUG: s_ivo = { %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d }\n",
			s_ivo.mode,
			s_ivo.fullscreen,
			s_ivo.tq,
			s_ivo.lighting,
			s_ivo.colordepth,
			s_ivo.texturebits,
			s_ivo.geometry,
			s_ivo.filter,
			s_ivo.multisample,
			s_ivo.flares ? "qtrue" : "qfalse",
			s_ivo.fps
			);
#endif
}

/*
=================
GraphicsOptions_GetResolutions
=================
*/
static void GraphicsOptions_GetResolutions( void )
{
	trap_Cvar_VariableStringBuffer("r_availableModes", resbuf, sizeof(resbuf));
	if(*resbuf)
	{
		char* s = resbuf;
		unsigned int i = 0;
		static char displayRes[64];

		// Add display resolution video mode
		Com_sprintf(displayRes, sizeof(displayRes), "Auto (%dx%d)", cgs.glconfig.displayWidth, cgs.glconfig.displayHeight);
		detectedResolutions[i++] = displayRes;

		// Use display resolution in "Very High Quality" template
		s_ivo_templates[0].mode = -2;

		while( s && i < ARRAY_LEN(detectedResolutions)-1 )
		{
			detectedResolutions[i++] = s;
			s = strchr(s, ' ');
			if( s )
				*s++ = '\0';
		}
		detectedResolutions[ i ] = NULL;

		// add custom resolution if not in mode list
		if ( i < ARRAY_LEN(detectedResolutions)-1 )
		{
			Com_sprintf( currentResolution, sizeof ( currentResolution ), "%dx%d", cgs.glconfig.vidWidth, cgs.glconfig.vidHeight );

			for( i = 0; detectedResolutions[ i ]; i++ )
			{
				if ( strcmp( detectedResolutions[ i ], currentResolution ) == 0 )
					break;
			}

			if ( detectedResolutions[ i ] == NULL )
			{
				detectedResolutions[ i++ ] = currentResolution;
				detectedResolutions[ i ] = NULL;
			}
		}

		resolutions = detectedResolutions;
		resolutionsDetected = qtrue;
	}
}

/*
=================
GraphicsOptions_CheckConfig
=================
*/
static void GraphicsOptions_CheckConfig( void )
{
	int i;

	for ( i = 0; i < NUM_IVO_TEMPLATES-1; i++ )
	{
		if ( s_ivo_templates[i].colordepth != s_graphicsoptions.colordepth.curvalue )
			continue;
		if ( GraphicsOptions_FindDetectedResolution(s_ivo_templates[i].mode) != s_graphicsoptions.mode.curvalue )
			continue;
		if ( s_ivo_templates[i].fps != s_graphicsoptions.fps.curvalue )
			continue;
//		if ( s_ivo_templates[i].fullscreen != s_graphicsoptions.fs.curvalue )
//			continue;
		if ( s_ivo_templates[i].tq != s_graphicsoptions.tq.curvalue )
			continue;
		if ( s_ivo_templates[i].lighting != s_graphicsoptions.lighting.curvalue )
			continue;
		if ( s_ivo_templates[i].flares != s_graphicsoptions.flares.curvalue )
			continue;
		if ( s_ivo_templates[i].geometry != s_graphicsoptions.geometry.curvalue )
			continue;
		if ( s_ivo_templates[i].filter != s_graphicsoptions.filter.curvalue )
			continue;
		if ( s_ivo_templates[i].multisample != s_graphicsoptions.multisample.curvalue )
			continue;
//		if ( s_ivo_templates[i].texturebits != s_graphicsoptions.texturebits.curvalue )
//			continue;
		s_graphicsoptions.list.curvalue = i;
		return;
	}

	// return 'Custom' ivo template
	s_graphicsoptions.list.curvalue = NUM_IVO_TEMPLATES - 1;
}

/*
=================
GraphicsOptions_UpdateMenuItems
=================
*/
static void GraphicsOptions_UpdateMenuItems( void )
{
	if ( s_graphicsoptions.fs.curvalue == 0 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
		s_graphicsoptions.colordepth.generic.flags |= QMF_GRAYED;
	}
	else
	{
		s_graphicsoptions.colordepth.generic.flags &= ~QMF_GRAYED;
	}

	s_graphicsoptions.apply.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

	if ( s_ivo.mode != s_graphicsoptions.mode.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.fps != s_graphicsoptions.fps.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.fullscreen != s_graphicsoptions.fs.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.tq != s_graphicsoptions.tq.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.lighting != s_graphicsoptions.lighting.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.colordepth != s_graphicsoptions.colordepth.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.flares != s_graphicsoptions.flares.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.texturebits != s_graphicsoptions.texturebits.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.geometry != s_graphicsoptions.geometry.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.filter != s_graphicsoptions.filter.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_ivo.multisample != s_graphicsoptions.multisample.curvalue )
	{
		s_graphicsoptions.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}

	GraphicsOptions_CheckConfig();
}	

/*
=================
GraphicsOptions_ApplyChanges
=================
*/
static void GraphicsOptions_ApplyChanges( void *unused, int notification )
{
	if (notification != QM_ACTIVATED)
		return;

	switch ( s_graphicsoptions.texturebits.curvalue  )
	{
	case 0:
		trap_Cvar_SetValue( "r_texturebits", 0 );
		break;
	case 1:
		trap_Cvar_SetValue( "r_texturebits", 16 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_texturebits", 32 );
		break;
	}
	trap_Cvar_SetValue( "r_picmip", 3 - s_graphicsoptions.tq.curvalue );

	if( resolutionsDetected )
	{
		// search for builtin mode that matches the detected mode
		int mode;
		if ( s_graphicsoptions.mode.curvalue == -1
			|| s_graphicsoptions.mode.curvalue >= ARRAY_LEN( detectedResolutions ) )
			s_graphicsoptions.mode.curvalue = 0;

		mode = GraphicsOptions_FindBuiltinResolution( s_graphicsoptions.mode.curvalue );
		if( mode == -1 )
		{
			char w[ 16 ], h[ 16 ];
			Q_strncpyz( w, detectedResolutions[ s_graphicsoptions.mode.curvalue ], sizeof( w ) );
			*strchr( w, 'x' ) = 0;
			Q_strncpyz( h,
					strchr( detectedResolutions[ s_graphicsoptions.mode.curvalue ], 'x' ) + 1, sizeof( h ) );
			trap_Cvar_Set( "r_customwidth", w );
			trap_Cvar_Set( "r_customheight", h );
		}

		trap_Cvar_SetValue( "r_mode", mode );
	}
	else
		trap_Cvar_SetValue( "r_mode", s_graphicsoptions.mode.curvalue );

	switch ( s_graphicsoptions.fps.curvalue )
	{
	case 0:
		trap_Cvar_SetValue( "r_swapInterval", 1 );
		trap_Cvar_SetValue( "com_maxfps", 125 );
		break;
	case 1:
		trap_Cvar_SetValue( "r_swapInterval", 0 );
		trap_Cvar_SetValue( "com_maxfps", 30 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_swapInterval", 0 );
		trap_Cvar_SetValue( "com_maxfps", 60 );
		break;
	case 3:
		trap_Cvar_SetValue( "r_swapInterval", 0 );
		trap_Cvar_SetValue( "com_maxfps", 85 );
		break;
	case 4:
		trap_Cvar_SetValue( "r_swapInterval", 0 );
		trap_Cvar_SetValue( "com_maxfps", 125 );
		break;
	}

	trap_Cvar_SetValue( "r_fullscreen", s_graphicsoptions.fs.curvalue );
	switch ( s_graphicsoptions.colordepth.curvalue )
	{
	case 0:
		trap_Cvar_SetValue( "r_colorbits", 0 );
		trap_Cvar_SetValue( "r_depthbits", 0 );
		trap_Cvar_Reset( "r_stencilbits" );
		break;
	case 1:
		trap_Cvar_SetValue( "r_colorbits", 16 );
		trap_Cvar_SetValue( "r_depthbits", 16 );
		trap_Cvar_SetValue( "r_stencilbits", 0 );
		break;
	case 2:
		trap_Cvar_SetValue( "r_colorbits", 32 );
		trap_Cvar_SetValue( "r_depthbits", 24 );
		trap_Cvar_SetValue( "r_stencilbits", 8 );
		break;
	}
	trap_Cvar_SetValue( "r_vertexLight", s_graphicsoptions.lighting.curvalue );
	trap_Cvar_SetValue( "r_flares", s_graphicsoptions.flares.curvalue );

	if ( s_graphicsoptions.geometry.curvalue == 4 )
	{
		trap_Cvar_SetValue( "r_lodBias", -2 );
		trap_Cvar_SetValue( "r_subdivisions", 1 );
		trap_Cvar_SetValue( "r_lodCurveError", 10000 );
	}
	else if ( s_graphicsoptions.geometry.curvalue == 3 )
	{
		trap_Cvar_SetValue( "r_lodBias", -2 );
		trap_Cvar_SetValue( "r_subdivisions", 4 );
		trap_Cvar_SetValue( "r_lodCurveError", 1000 );
	}
	else if ( s_graphicsoptions.geometry.curvalue == 2 )
	{
		trap_Cvar_SetValue( "r_lodBias", 0 );
		trap_Cvar_SetValue( "r_subdivisions", 4 );
		trap_Cvar_SetValue( "r_lodCurveError", 250 );
	}
	else if ( s_graphicsoptions.geometry.curvalue == 1 )
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 12 );
		trap_Cvar_SetValue( "r_lodCurveError", 250 );
	}
	else
	{
		trap_Cvar_SetValue( "r_lodBias", 1 );
		trap_Cvar_SetValue( "r_subdivisions", 20 );
		trap_Cvar_SetValue( "r_lodCurveError", 250 );
	}

	if ( s_graphicsoptions.filter.curvalue >= 2 )
	{
		trap_Cvar_SetValue( "r_ext_texture_filter_anisotropic", 1 );

		switch ( s_graphicsoptions.filter.curvalue )
		{
			default:
			case 2:
				trap_Cvar_SetValue( "r_ext_max_anisotropy", 2 );
				break;
			case 3:
				trap_Cvar_SetValue( "r_ext_max_anisotropy", 4 );
				break;
			case 4:
				trap_Cvar_SetValue( "r_ext_max_anisotropy", 8 );
				break;
			case 5:
				trap_Cvar_SetValue( "r_ext_max_anisotropy", 16 );
				break;
		}
	}
	else
	{
		trap_Cvar_SetValue( "r_ext_texture_filter_anisotropic", 0 );

		if ( s_graphicsoptions.filter.curvalue )
		{
			trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_LINEAR" );
		}
		else
		{
			trap_Cvar_Set( "r_textureMode", "GL_LINEAR_MIPMAP_NEAREST" );
		}
	}

	trap_Cvar_SetValue( "r_ext_multisample", s_graphicsoptions.multisample.curvalue * 2 );
	trap_Cvar_SetValue( "r_ext_framebuffer_multisample", s_graphicsoptions.multisample.curvalue * 2 );

	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
}

/*
=================
GraphicsOptions_Event
=================
*/
static void GraphicsOptions_Event( void* ptr, int event ) {
	InitialVideoOptions_s *ivo;

	if( event != QM_ACTIVATED ) {
	 	return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_RATIO:
		s_graphicsoptions.mode.curvalue =
			ratioToRes[ s_graphicsoptions.ratio.curvalue ];
		// fall through to apply mode constraints
		
	case ID_MODE:
		s_graphicsoptions.ratio.curvalue =
			resToRatio[ s_graphicsoptions.mode.curvalue ];
		break;

	case ID_LIST:
		ivo = &s_ivo_templates[s_graphicsoptions.list.curvalue];

		s_graphicsoptions.mode.curvalue        = GraphicsOptions_FindDetectedResolution(ivo->mode);
		s_graphicsoptions.ratio.curvalue =
			resToRatio[ s_graphicsoptions.mode.curvalue ];
		s_graphicsoptions.fps.curvalue         = ivo->fps;
		s_graphicsoptions.tq.curvalue          = ivo->tq;
		s_graphicsoptions.lighting.curvalue    = ivo->lighting;
		s_graphicsoptions.colordepth.curvalue  = ivo->colordepth;
		s_graphicsoptions.flares.curvalue      = ivo->flares;
		s_graphicsoptions.texturebits.curvalue = ivo->texturebits;
		s_graphicsoptions.geometry.curvalue    = ivo->geometry;
		s_graphicsoptions.filter.curvalue      = ivo->filter;
		s_graphicsoptions.multisample.curvalue = ivo->multisample;
		s_graphicsoptions.fs.curvalue          = ivo->fullscreen;
		break;

	case ID_DRIVERINFO:
		UI_DriverInfo_Menu();
		break;

	case ID_BACK2:
		UI_PopMenu();
		break;

	case ID_GRAPHICS:
		break;

	case ID_DISPLAY:
		UI_PopMenu();
		UI_DisplayOptionsMenu();
		break;

	case ID_SOUND:
		UI_PopMenu();
		UI_SoundOptionsMenu();
		break;

	case ID_NETWORK:
		UI_PopMenu();
		UI_NetworkOptionsMenu();
		break;
	}
}


/*
================
GraphicsOptions_MenuDraw
================
*/
void GraphicsOptions_MenuDraw (void)
{
//APSFIX - rework this
	GraphicsOptions_UpdateMenuItems();

	Menu_Draw( &s_graphicsoptions.menu );
}

/*
=================
GraphicsOptions_SetMenuItems
=================
*/
static void GraphicsOptions_SetMenuItems( void )
{
	int multisample;

	s_graphicsoptions.mode.curvalue =
		GraphicsOptions_FindDetectedResolution( trap_Cvar_VariableValue( "r_mode" ) );

	if ( s_graphicsoptions.mode.curvalue < 0 )
	{
		if( resolutionsDetected )
		{
			int i;
			char buf[MAX_STRING_CHARS];
			trap_Cvar_VariableStringBuffer("r_customwidth", buf, sizeof(buf)-2);
			buf[strlen(buf)+1] = 0;
			buf[strlen(buf)] = 'x';
			trap_Cvar_VariableStringBuffer("r_customheight", buf+strlen(buf), sizeof(buf)-strlen(buf));

			for(i = 0; detectedResolutions[i]; ++i)
			{
				if(!Q_stricmp(buf, detectedResolutions[i]))
				{
					s_graphicsoptions.mode.curvalue = i;
					break;
				}
			}
			if ( s_graphicsoptions.mode.curvalue < 0 )
				s_graphicsoptions.mode.curvalue = 0;
		}
		else
		{
			s_graphicsoptions.mode.curvalue = 3;
		}
	}
	s_graphicsoptions.ratio.curvalue =
		resToRatio[ s_graphicsoptions.mode.curvalue ];

	s_graphicsoptions.fps.curvalue = trap_Cvar_VariableValue( "com_maxfps");
	if ( trap_Cvar_VariableValue( "r_swapInterval" ) != 0 )
	{
		s_graphicsoptions.fps.curvalue = 0;
	}
	else if ( s_graphicsoptions.fps.curvalue <= 30 )
	{
		s_graphicsoptions.fps.curvalue = 1;
	}
	else if ( s_graphicsoptions.fps.curvalue <= 60 )
	{
		s_graphicsoptions.fps.curvalue = 2;
	}
	else if ( s_graphicsoptions.fps.curvalue <= 85 )
	{
		s_graphicsoptions.fps.curvalue = 3;
	}
	else //if ( s_graphicsoptions.fps.curvalue <= 125 )
	{
		s_graphicsoptions.fps.curvalue = 4;
	}

	s_graphicsoptions.fs.curvalue = trap_Cvar_VariableValue("r_fullscreen");
	s_graphicsoptions.tq.curvalue = 3-trap_Cvar_VariableValue( "r_picmip");
	if ( s_graphicsoptions.tq.curvalue < 0 )
	{
		s_graphicsoptions.tq.curvalue = 0;
	}
	else if ( s_graphicsoptions.tq.curvalue > 3 )
	{
		s_graphicsoptions.tq.curvalue = 3;
	}

	s_graphicsoptions.lighting.curvalue = trap_Cvar_VariableValue( "r_vertexLight" ) != 0;
	s_graphicsoptions.flares.curvalue = trap_Cvar_VariableValue( "r_flares" ) != 0;

	switch ( ( int ) trap_Cvar_VariableValue( "r_texturebits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.texturebits.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.texturebits.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.texturebits.curvalue = 2;
		break;
	}

	if ( trap_Cvar_VariableIntegerValue( "r_ext_texture_filter_anisotropic" ) != 0 )
	{
		switch ( ( int ) trap_Cvar_VariableValue( "r_ext_max_anisotropy" ) )
		{
		default:
		case 2:
			s_graphicsoptions.filter.curvalue = 2;
			break;
		case 4:
			s_graphicsoptions.filter.curvalue = 3;
			break;
		case 8:
			s_graphicsoptions.filter.curvalue = 4;
			break;
		case 16:
			s_graphicsoptions.filter.curvalue = 5;
			break;
		}
	}
	else if ( !Q_stricmp( CG_Cvar_VariableString( "r_textureMode" ), "GL_LINEAR_MIPMAP_NEAREST" ) )
	{
		s_graphicsoptions.filter.curvalue = 0;
	}
	else
	{
		s_graphicsoptions.filter.curvalue = 1;
	}

	if ( trap_Cvar_VariableValue( "r_lodBias" ) > 0 )
	{
		if ( trap_Cvar_VariableValue( "r_subdivisions" ) >= 20 )
		{
			s_graphicsoptions.geometry.curvalue = 0;
		}
		else
		{
			s_graphicsoptions.geometry.curvalue = 1;
		}
	}
	else if ( trap_Cvar_VariableValue( "r_lodBias" ) == 0 )
	{
		s_graphicsoptions.geometry.curvalue = 2;
	}
	else
	{
		if ( trap_Cvar_VariableValue( "r_subdivisions" ) == 1 )
		{
			s_graphicsoptions.geometry.curvalue = 4;
		}
		else
		{
			s_graphicsoptions.geometry.curvalue = 3;
		}
	}

	switch ( ( int ) trap_Cvar_VariableValue( "r_colorbits" ) )
	{
	default:
	case 0:
		s_graphicsoptions.colordepth.curvalue = 0;
		break;
	case 16:
		s_graphicsoptions.colordepth.curvalue = 1;
		break;
	case 32:
		s_graphicsoptions.colordepth.curvalue = 2;
		break;
	}

	if ( s_graphicsoptions.fs.curvalue == 0 )
	{
		s_graphicsoptions.colordepth.curvalue = 0;
	}

	// use higher multisample value
	// r_ext_framebuffer_multisample is currently only used by the OpenGL2 renderer
	multisample = MAX( trap_Cvar_VariableIntegerValue( "r_ext_multisample" ),
					   trap_Cvar_VariableIntegerValue( "r_ext_framebuffer_multisample" ) );

	if ( multisample < 2 ) {
		// Off
		s_graphicsoptions.multisample.curvalue = 0;
	} else if ( multisample < 4 ) {
		// 2x MSAA
		s_graphicsoptions.multisample.curvalue = 1;
	} else {
		// 4x MSAA
		s_graphicsoptions.multisample.curvalue = 2;
	}
}

/*
=================
GraphicsOptions_StatusBar
=================
*/
static void GraphicsOptions_StatusBar( void *ptr ) {
	switch( ((menucommon_s*)ptr)->id ) {
	case ID_FPS:
		UI_DrawString( 320, 410, "The frame rate also affects your player's movement.", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 430, "125Hz can jump slightly higher than the others.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	default:
		break;
	}
}

/*
================
GraphicsOptions_MenuInit
================
*/
void GraphicsOptions_MenuInit( void )
{
	static const char *tq_names[] =
	{
		"Default",
		"16 bit",
		"32 bit",
		NULL
	};

	static const char *s_graphics_options_names[] =
	{
		"Very High Quality",
		"High Quality",
		"Normal",
		"Fast",
		"Fastest",
		"Custom",
		NULL
	};

	static const char *fps_names[] =
	{
		"V-Sync",
		"30 Hz",
		"60 Hz",
		"85 Hz",
		"125 Hz",
		NULL
	};

	static const char *lighting_names[] =
	{
		"Lightmap (High)",
		"Vertex (Low)",
		NULL
	};

	static const char *colordepth_names[] =
	{
		"Default",
		"16 bit",
		"32 bit",
		NULL
	};

	static const char *filter_names[] =
	{
		"Bilinear",
		"Trilinear",
		"Anisotropic 2x",
		"Anisotropic 4x",
		"Anisotropic 8x",
		"Anisotropic 16x",
		NULL
	};
	static const char *quality_names[] =
	{
		"Low",
		"Medium",
		"Standard",
		"High",
		"Very High",
		NULL
	};
	static const char *enabled_names[] =
	{
		"Off",
		"On",
		NULL
	};
	static const char *multisample_names[] =
	{
		"Off",
		"2x MSAA",
		"4x MSAA",
		NULL
	};
	static const char *detail_names[] =
	{
		"13%",
		"25%",
		"50%",
		"100%",
		NULL
	};

	int y;

	// zero set all our globals
	memset( &s_graphicsoptions, 0 ,sizeof(graphicsoptions_t) );

	GraphicsOptions_GetResolutions();
	GraphicsOptions_GetAspectRatios();
	
	GraphicsOptions_Cache();

	s_graphicsoptions.menu.wrapAround = qtrue;
	s_graphicsoptions.menu.fullscreen = qtrue;
	s_graphicsoptions.menu.draw       = GraphicsOptions_MenuDraw;

	s_graphicsoptions.banner.generic.type  = MTYPE_BTEXT;
	s_graphicsoptions.banner.generic.x	   = 320;
	s_graphicsoptions.banner.generic.y	   = 16;
	s_graphicsoptions.banner.string  	   = "SYSTEM SETUP";
	s_graphicsoptions.banner.color         = text_banner_color;
	s_graphicsoptions.banner.style         = UI_CENTER;

	s_graphicsoptions.framel.generic.type  = MTYPE_BITMAP;
	s_graphicsoptions.framel.generic.name  = GRAPHICSOPTIONS_FRAMEL;
	s_graphicsoptions.framel.generic.flags = QMF_INACTIVE;
	s_graphicsoptions.framel.generic.x	   = 0;
	s_graphicsoptions.framel.generic.y	   = 78;
	s_graphicsoptions.framel.width  	   = 256;
	s_graphicsoptions.framel.height  	   = 329;

	s_graphicsoptions.framer.generic.type  = MTYPE_BITMAP;
	s_graphicsoptions.framer.generic.name  = GRAPHICSOPTIONS_FRAMER;
	s_graphicsoptions.framer.generic.flags = QMF_INACTIVE;
	s_graphicsoptions.framer.generic.x	   = 376;
	s_graphicsoptions.framer.generic.y	   = 76;
	s_graphicsoptions.framer.width  	   = 256;
	s_graphicsoptions.framer.height  	   = 334;

	s_graphicsoptions.graphics.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.graphics.generic.flags	= QMF_RIGHT_JUSTIFY;
	s_graphicsoptions.graphics.generic.id		= ID_GRAPHICS;
	s_graphicsoptions.graphics.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.graphics.generic.x		= 216;
	s_graphicsoptions.graphics.generic.y		= 240 - 2 * PROP_HEIGHT;
	s_graphicsoptions.graphics.string			= "GRAPHICS";
	s_graphicsoptions.graphics.style			= UI_RIGHT;
	s_graphicsoptions.graphics.color			= text_big_color;

	s_graphicsoptions.display.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.display.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.display.generic.id		= ID_DISPLAY;
	s_graphicsoptions.display.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.display.generic.x			= 216;
	s_graphicsoptions.display.generic.y			= 240 - PROP_HEIGHT;
	s_graphicsoptions.display.string			= "DISPLAY";
	s_graphicsoptions.display.style				= UI_RIGHT;
	s_graphicsoptions.display.color				= text_big_color;

	s_graphicsoptions.sound.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.sound.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.sound.generic.id			= ID_SOUND;
	s_graphicsoptions.sound.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.sound.generic.x			= 216;
	s_graphicsoptions.sound.generic.y			= 240;
	s_graphicsoptions.sound.string				= "SOUND";
	s_graphicsoptions.sound.style				= UI_RIGHT;
	s_graphicsoptions.sound.color				= text_big_color;

	s_graphicsoptions.network.generic.type		= MTYPE_PTEXT;
	s_graphicsoptions.network.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.network.generic.id		= ID_NETWORK;
	s_graphicsoptions.network.generic.callback	= GraphicsOptions_Event;
	s_graphicsoptions.network.generic.x			= 216;
	s_graphicsoptions.network.generic.y			= 240 + PROP_HEIGHT;
	s_graphicsoptions.network.string			= "NETWORK";
	s_graphicsoptions.network.style				= UI_RIGHT;
	s_graphicsoptions.network.color				= text_big_color;

	y = 240 - 7 * (BIGCHAR_HEIGHT + 2);
	s_graphicsoptions.list.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.list.generic.name     = "Graphics Settings:";
	s_graphicsoptions.list.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.list.generic.x        = 400;
	s_graphicsoptions.list.generic.y        = y;
	s_graphicsoptions.list.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.list.generic.id       = ID_LIST;
	s_graphicsoptions.list.itemnames        = s_graphics_options_names;
	y += 2 * ( BIGCHAR_HEIGHT + 2 );

	s_graphicsoptions.ratio.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.ratio.generic.name     = "Aspect Ratio:";
	s_graphicsoptions.ratio.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.ratio.generic.x        = 400;
	s_graphicsoptions.ratio.generic.y        = y;
	s_graphicsoptions.ratio.itemnames        = ratios;
	s_graphicsoptions.ratio.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.ratio.generic.id       = ID_RATIO;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_mode"
	s_graphicsoptions.mode.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.mode.generic.name     = "Resolution:";
	s_graphicsoptions.mode.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.mode.generic.x        = 400;
	s_graphicsoptions.mode.generic.y        = y;
	s_graphicsoptions.mode.itemnames        = resolutions;
	s_graphicsoptions.mode.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.mode.generic.id       = ID_MODE;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "com_maxfps" and "r_swapInterval"
	s_graphicsoptions.fps.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.fps.generic.name     = "Frame Rate:";
	s_graphicsoptions.fps.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.fps.generic.x        = 400;
	s_graphicsoptions.fps.generic.y        = y;
	s_graphicsoptions.fps.itemnames        = fps_names;
	s_graphicsoptions.fps.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.fps.generic.id       = ID_FPS;
	s_graphicsoptions.fps.generic.statusbar = GraphicsOptions_StatusBar;
	y += BIGCHAR_HEIGHT+2;

	// references "r_colorbits"
	s_graphicsoptions.colordepth.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.colordepth.generic.name     = "Color Depth:";
	s_graphicsoptions.colordepth.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.colordepth.generic.x        = 400;
	s_graphicsoptions.colordepth.generic.y        = y;
	s_graphicsoptions.colordepth.itemnames        = colordepth_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_fullscreen"
	s_graphicsoptions.fs.generic.type     = MTYPE_SPINCONTROL;
	s_graphicsoptions.fs.generic.name	  = "Fullscreen:";
	s_graphicsoptions.fs.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.fs.generic.x	      = 400;
	s_graphicsoptions.fs.generic.y	      = y;
	s_graphicsoptions.fs.itemnames	      = enabled_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_ext_multisample"
	s_graphicsoptions.multisample.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.multisample.generic.name  = "Anti-aliasing:";
	s_graphicsoptions.multisample.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.multisample.generic.x     = 400;
	s_graphicsoptions.multisample.generic.y     = y;
	s_graphicsoptions.multisample.itemnames     = multisample_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_vertexLight"
	s_graphicsoptions.lighting.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.lighting.generic.name	 = "Lighting:";
	s_graphicsoptions.lighting.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.lighting.generic.x	 = 400;
	s_graphicsoptions.lighting.generic.y	 = y;
	s_graphicsoptions.lighting.itemnames     = lighting_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_flares"
	s_graphicsoptions.flares.generic.type    = MTYPE_SPINCONTROL;
	s_graphicsoptions.flares.generic.name    = "Flares:";
	s_graphicsoptions.flares.generic.flags   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.flares.generic.x       = 400;
	s_graphicsoptions.flares.generic.y       = y;
	s_graphicsoptions.flares.itemnames       = enabled_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_lodBias" & "subdivisions"
	s_graphicsoptions.geometry.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.geometry.generic.name	 = "Geometric Detail:";
	s_graphicsoptions.geometry.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.geometry.generic.x	 = 400;
	s_graphicsoptions.geometry.generic.y	 = y;
	s_graphicsoptions.geometry.itemnames     = quality_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_picmip"
	s_graphicsoptions.tq.generic.type	= MTYPE_SPINCONTROL;
	s_graphicsoptions.tq.generic.name	= "Texture Detail:";
	s_graphicsoptions.tq.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.tq.generic.x		= 400;
	s_graphicsoptions.tq.generic.y		= y;
	s_graphicsoptions.tq.itemnames		= detail_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_textureBits"
	s_graphicsoptions.texturebits.generic.type  = MTYPE_SPINCONTROL;
	s_graphicsoptions.texturebits.generic.name	= "Texture Quality:";
	s_graphicsoptions.texturebits.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.texturebits.generic.x	    = 400;
	s_graphicsoptions.texturebits.generic.y	    = y;
	s_graphicsoptions.texturebits.itemnames     = tq_names;
	y += BIGCHAR_HEIGHT+2;

	// references/modifies "r_textureMode"
	s_graphicsoptions.filter.generic.type   = MTYPE_SPINCONTROL;
	s_graphicsoptions.filter.generic.name	= "Texture Filter:";
	s_graphicsoptions.filter.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_graphicsoptions.filter.generic.x	    = 400;
	s_graphicsoptions.filter.generic.y	    = y;
	s_graphicsoptions.filter.itemnames      = filter_names;
	y += 2*BIGCHAR_HEIGHT;

	s_graphicsoptions.driverinfo.generic.type     = MTYPE_PTEXT;
	s_graphicsoptions.driverinfo.generic.flags    = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.driverinfo.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.driverinfo.generic.id       = ID_DRIVERINFO;
	s_graphicsoptions.driverinfo.generic.x        = 320;
	s_graphicsoptions.driverinfo.generic.y        = y;
	s_graphicsoptions.driverinfo.string           = "Driver Info";
	s_graphicsoptions.driverinfo.style            = UI_CENTER|UI_SMALLFONT;
	s_graphicsoptions.driverinfo.color            = text_big_color;

	s_graphicsoptions.back.generic.type	    = MTYPE_BITMAP;
	s_graphicsoptions.back.generic.name     = GRAPHICSOPTIONS_BACK0;
	s_graphicsoptions.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_graphicsoptions.back.generic.callback = GraphicsOptions_Event;
	s_graphicsoptions.back.generic.id	    = ID_BACK2;
	s_graphicsoptions.back.generic.x		= 0;
	s_graphicsoptions.back.generic.y		= 480-64;
	s_graphicsoptions.back.width  		    = 128;
	s_graphicsoptions.back.height  		    = 64;
	s_graphicsoptions.back.focuspic         = GRAPHICSOPTIONS_BACK1;

	s_graphicsoptions.apply.generic.type     = MTYPE_BITMAP;
	s_graphicsoptions.apply.generic.name     = GRAPHICSOPTIONS_ACCEPT0;
	s_graphicsoptions.apply.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_HIDDEN|QMF_INACTIVE;
	s_graphicsoptions.apply.generic.callback = GraphicsOptions_ApplyChanges;
	s_graphicsoptions.apply.generic.x        = 640;
	s_graphicsoptions.apply.generic.y        = 480-64;
	s_graphicsoptions.apply.width  		     = 128;
	s_graphicsoptions.apply.height  		 = 64;
	s_graphicsoptions.apply.focuspic         = GRAPHICSOPTIONS_ACCEPT1;

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.banner );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.framel );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.framer );

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.graphics );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.display );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.sound );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.network );

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.list );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.ratio );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.mode );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.fps );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.colordepth );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.fs );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.multisample );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.lighting );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.flares );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.geometry );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.tq );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.texturebits );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.filter );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.driverinfo );

	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.back );
	Menu_AddItem( &s_graphicsoptions.menu, ( void * ) &s_graphicsoptions.apply );

	GraphicsOptions_SetMenuItems();
	GraphicsOptions_GetInitialVideo();
}


/*
=================
GraphicsOptions_Cache
=================
*/
void GraphicsOptions_Cache( void ) {
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_FRAMEL );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_FRAMER );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_BACK0 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_BACK1 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_ACCEPT0 );
	trap_R_RegisterShaderNoMip( GRAPHICSOPTIONS_ACCEPT1 );
}


/*
=================
UI_GraphicsOptionsMenu
=================
*/
void UI_GraphicsOptionsMenu( void ) {
	GraphicsOptions_MenuInit();
	UI_PushMenu( &s_graphicsoptions.menu );
	Menu_SetCursorToItem( &s_graphicsoptions.menu, &s_graphicsoptions.graphics );
}

